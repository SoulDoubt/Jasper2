#ifndef _JASPER_SKYBOX_RENDERER_H_
#define _JASPER_SKYBOX_RENDERER_H_


#include "MeshRenderer.h"

namespace Jasper
{

class SkyboxRenderer :
    public MeshRenderer
{
public:
    explicit SkyboxRenderer(std::string name, Mesh* mesh, Material* mat);
    SkyboxRenderer(const SkyboxRenderer& o) = delete;
    SkyboxRenderer& operator=(const SkyboxRenderer& o) = delete;
    ~SkyboxRenderer();
    void Update(double dt) override;
    void Render() override;
    ComponentType GetComponentType() const override {
        return ComponentType::SkyboxRenderer;
    }
	void Serialize(std::ofstream& ofs) const override;

};

}

#endif // _JASPER_SKYBOX_RENDERER_H_
