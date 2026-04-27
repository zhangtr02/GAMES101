//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    Intersection inter = intersect(ray);
    if (!inter.happened)
        return Vector3f(0.0f);

    if (inter.m->hasEmission())
        return inter.m->getEmission();

    Vector3f hitPoint = inter.coords;
    Vector3f N = inter.normal;
    Vector3f wo = ray.direction;

    Vector3f L_dir(0.0f), L_indir(0.0f);

    Intersection lightInter;
    float lightPdf = 0.0f;
    sampleLight(lightInter, lightPdf);

    Vector3f lightDir = normalize(lightInter.coords - hitPoint);
    float lightDistance2 = dotProduct(lightInter.coords - hitPoint,
                                      lightInter.coords - hitPoint);
    Vector3f shadowOrigin = dotProduct(lightDir, N) > 0
                                ? hitPoint + N * EPSILON
                                : hitPoint - N * EPSILON;
    Intersection shadowInter = intersect(Ray(shadowOrigin, lightDir));

    if (shadowInter.happened &&
        (shadowInter.coords - lightInter.coords).norm() < EPSILON &&
        lightPdf > EPSILON)
    {
        float cosTheta = std::max(0.0f, dotProduct(lightDir, N));
        float cosThetaLight = std::max(0.0f, dotProduct(-lightDir, lightInter.normal));
        L_dir = lightInter.emit * inter.m->eval(wo, lightDir, N) *
                cosTheta * cosThetaLight / lightDistance2 / lightPdf;
    }

    if (get_random_float() < RussianRoulette)
    {
        Vector3f sampleDir = normalize(inter.m->sample(wo, N));
        Vector3f sampleOrigin = dotProduct(sampleDir, N) > 0
                                    ? hitPoint + N * EPSILON
                                    : hitPoint - N * EPSILON;
        Intersection sampleInter = intersect(Ray(sampleOrigin, sampleDir));
        float samplePdf = inter.m->pdf(wo, sampleDir, N);

        if (sampleInter.happened && !sampleInter.m->hasEmission() && samplePdf > EPSILON)
        {
            L_indir = castRay(Ray(sampleOrigin, sampleDir), depth + 1) *
                      inter.m->eval(wo, sampleDir, N) *
                      std::max(0.0f, dotProduct(sampleDir, N)) /
                      samplePdf / RussianRoulette;
        }
    }

    return L_dir + L_indir;
}
