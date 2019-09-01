#pragma once

class TileMaterial : public Material {
	void Bind(AssetRef<Shader> shader) override;
	void Unbind() override;
};