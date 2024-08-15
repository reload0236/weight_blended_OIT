#include <memory>
#include <random>

#include "SceneCreator.h"
#include "VBObjectExt.h"
#include "vbm.h"

bool SceneCreator::CreateOpaqueObjects(std::vector<std::shared_ptr<VBObjectExt>>& out)
{
    out.clear();

    // ����һ�����������
    std::random_device rd; // ���ڻ�ȡ�������
    std::mt19937 gen(rd()); // ��random_device()��Ϊ���ӵ�Mersenne Twister������
    // ����һ����[0.0, 1.0)�����ھ��ȷֲ���������ֲ�
    std::uniform_real_distribution<> dis(0.0, 1.0);

    const float STEP = 50;
    const float LENGTH = 400;
    const float YOFFSET = .0;

    out.clear();
    float startX = -LENGTH * 0.5f;
    float startZ = -LENGTH * 0.5f;
    for (float z = .0; z < LENGTH; z += STEP) {
        for (float x = .0; x < LENGTH; x += STEP) {
            std::shared_ptr<VBObjectExt> obj(new VBObjectExt());
            obj->LoadFromVBM("media/torus.vbm", 0, 1, 2);
            obj->SetColor(dis(gen), dis(gen), dis(gen), 1.0);
            obj->Translate(startX + x, YOFFSET, startZ + z);
            out.push_back(obj);
        }
    }

    return true;
}

bool SceneCreator::CreateTranslucentObjects(std::vector<std::shared_ptr<VBObjectExt>>& out)
{
    out.clear();

    // ����һ�����������
    std::random_device rd; // ���ڻ�ȡ�������
    std::mt19937 gen(rd()); // ��random_device()��Ϊ���ӵ�Mersenne Twister������
    // ����һ����[0.0, 1.0)�����ھ��ȷֲ���������ֲ�
    std::uniform_real_distribution<> dis(0.0, 1.0);

    const float STEP = 50;
    const float LENGTH = 400;
    const float YOFFSET = 100.0;

    out.clear();
    float startX = -LENGTH * 0.5f;
    float startZ = -LENGTH * 0.5f;
    for (float z = .0; z < LENGTH; z += STEP) {
        for (float x = .0; x < LENGTH; x += STEP) {
            std::shared_ptr<VBObjectExt> obj(new VBObjectExt());
            obj->LoadFromVBM("media/torus.vbm", 0, 1, 2);
            obj->SetColor(dis(gen), dis(gen), dis(gen), dis(gen));
            obj->Translate(startX + x, YOFFSET, startZ + z);
            out.push_back(obj);
        }
    }

    return true;
}