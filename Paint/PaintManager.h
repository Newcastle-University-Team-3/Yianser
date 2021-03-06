#pragma once
#include "PaintableRenderObject.h"
#include "PaintableSurface.h"

class PaintManager
{
	class PaintableRenderObject;
public:
	static PaintManager* init();
	static void paint(const PaintableSurface* surface, const NCL::Maths::Vector3& position, float radius = 1.0f, float hardness = 0.5f, float strength = 0.5f,
	                  const NCL::Maths::Vector3& colour = NCL::Maths::Vector3(1, 0, 0));
protected:
	static PaintManager* instance_;
	PaintManager();
	NCL::Rendering::ShaderBase* paint_instance_shader_;
};


