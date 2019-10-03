#pragma once

class Material {
protected:
	friend class MaterialManager;

public:
	virtual void Bind(AssetRef<Shader> shader) = 0;
	//virtual void Unbind() = 0;
};