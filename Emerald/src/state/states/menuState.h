#pragma once

struct DrawableLine {
	Vector3 x1 = Vector3(0);
	Vector3 x2 = Vector3(0);
	float amount = 0;
	float duration = 0;
	bool enabled = false;
};

static DrawableLine** drawableLines;

static void DrawLine(Vector3 x1, Vector3 x2, float amount) {
	DrawableLine* linea;
	for (int i = 0; i < 128; i++) {
		DrawableLine* line = drawableLines[i];
		if (!line->enabled) {
			line->enabled = true;
			line->x1 = x1;
			linea = line;
			line->x2 = x2;
			line->amount = amount;
			line->duration = 100000;
			break;
		}
	}
}

struct Topology {
	int count;
	int layers;
};

class Neuron;

struct Connection {
	double weight;
	double deltaWeight;
	Neuron* neuron;
	double passingValue = 1;
};

class Neuron {
public:
	static double randomWeight() { return rand() / double(RAND_MAX); }

	Neuron(uint numOutputs, uint myIndex, float x, float y, float z) : m_myIndex(myIndex) {
		Model* model = NEW(Model(GetAssetManager()->Get<Model>("Sphere")->GetMeshes()[0]->Copy()));
		m_material = GetMaterialManager()->CreateUnsafe<BasicMaterial>();
		m_material->SetAlbedo(GetAssetManager()->Get<Texture>("White"));
		m_material->SetSpecular(GetAssetManager()->Get<Texture>("Spec"));
		model->SetMaterial(m_material);
		m_entity = NEW(Entity(model));
		m_entity->m_position = Vector3(x, y, z);
		for (uint c = 0; c < numOutputs; ++c) {
			m_outputWeights.push_back(Connection());
			m_outputWeights.back().weight = randomWeight();
		}
	}

	void setOutputValue(double val) { m_outputValue = val; }
	double getOutputValue() const { return m_outputValue; }
	void feedForward(vector<Neuron>& previousLayer) {
		double sum = 0.0;

		for (uint n = 0; n < previousLayer.size(); ++n) {
			Neuron& neuron = previousLayer[n];
			double passingValue = previousLayer[n].m_outputValue * previousLayer[n].m_outputWeights[m_myIndex].weight;
			sum += passingValue;
			neuron.m_outputWeights[m_myIndex].passingValue = passingValue;
			DrawLine(m_entity->m_position, neuron.m_entity->m_position, 100);
		}

		m_outputValue = transferFunction(sum);
	}

	double sumDOW(const vector<Neuron>& nextLayer) const {
		double sum = 0.0;
		for (uint n = 0; n < nextLayer.size() - 1; ++n) {
			sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
		}
		return sum;
	}
	void calcOutputGradients(double targetVal) {
		double delta = targetVal - m_outputValue;
		m_gradient = delta * Neuron::transferFunctionDerivative(m_outputValue);
	}

	void calcHiddenGradients(const vector<Neuron>& nextLayer) {
		double dow = sumDOW(nextLayer);
		m_gradient = dow * Neuron::transferFunctionDerivative(m_outputValue);
	}

	void updateInputWeights(vector<Neuron>& prevLayer) {
		for (uint n = 0; n < prevLayer.size(); ++n) {
			Neuron& neuron = prevLayer[n];
			double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

			double newDeltaWeight = eta * neuron.getOutputValue() * m_gradient + alpha * oldDeltaWeight;
			neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
			neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
		}
	}

	void setConnections(vector<Neuron>& prevLayer) {
		for (uint n = 0; n < prevLayer.size(); ++n) {
			Neuron& neuron = prevLayer[n];
			neuron.m_outputWeights[m_myIndex].neuron = this;
		}
	}

	void Draw(AssetRef<Shader>& shader, bool renderType) {
		Color color(0, 0, 0, 1);
		if (m_outputValue < 0) color.B = -m_outputValue;
		else color.R = m_outputValue;

		m_material->m_color = color;
		m_entity->Draw(shader, GL_PATCHES);

		for (auto& c : m_outputWeights) {
			float value = renderType ? c.weight : c.passingValue;
			Color color(0, 0, 0, 1);
			if (c.weight < 0) color.B = -value;
			else color.R = value;
		
			GetLineRenderer()->Submit(m_entity->m_position, c.neuron->m_entity->m_position, color);
		}
	}

	void Export(nlohmann::json& jsonOb) {
		for (int i = 0; i < m_outputWeights.size(); i++) {
			jsonOb[to_string(i)]["w"] = m_outputWeights[i].weight;
			jsonOb[to_string(i)]["d"] = m_outputWeights[i].deltaWeight;
			jsonOb[to_string(i)]["v"] = m_outputValue;
			jsonOb[to_string(i)]["g"] = m_gradient;
		}
	}

	void Import(nlohmann::json& jsonOb) {
		for (int i = 0; i < m_outputWeights.size(); i++) {
			m_outputWeights[i].weight = jsonOb[to_string(i)]["w"];
			m_outputWeights[i].deltaWeight = jsonOb[to_string(i)]["d"];
			m_outputValue = jsonOb[to_string(i)]["v"];
			m_gradient = jsonOb[to_string(i)]["g"];
		}
	}

private:
	double m_outputValue;
	double m_gradient;
	double eta = 0.15;
	double alpha = 0.5;
	uint m_myIndex;
	vector<Connection> m_outputWeights;
	AssetRef<Entity> m_entity;
	AssetRef<BasicMaterial> m_material;
	static double transferFunction(double x) {
		return tanh(x);
	}

	static double transferFunctionDerivative(double x) {
		return 1.0 - tanh(x) * tanh(x);
	}
};

class NeuralNetwork {
public:
	NeuralNetwork(const vector<Topology>& topology) {
		uint numLayers = topology.size();
		for (int layerNum = 0; layerNum < numLayers; ++layerNum) {
			m_layers.push_back(vector<Neuron>());
			uint numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1].count;
			for (int neuronNum = 0; neuronNum <= topology[layerNum].count; ++neuronNum) {
				Topology topo = topology[layerNum];
				float xPos = ((int)((float)neuronNum / topo.layers)) - (((float)topo.count / topo.layers) / 2);
				int layerSizeX = topo.layers;
				float yPos = (neuronNum % topo.layers) - ((float)layerSizeX / 2);
				m_layers[layerNum].push_back(Neuron(numOutputs, neuronNum, xPos, yPos, layerNum * 3 + 0.75f));
			}
			m_layers.back().back().setOutputValue(1.0);
		}

		for (uint layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
			vector<Neuron>& layer = m_layers[layerNum];
			vector<Neuron>& previousLayer = m_layers[layerNum + -1];

			for (uint n = 0; n < layer.size() - 1; ++n) {
				layer[n].setConnections(previousLayer);
			}
		}
	}

	void Export() {
		nlohmann::json jsonOb;
		for (int i = 0; i < m_layers.size() - 1; i++) {
			for (int j = 0; j < m_layers[i].size(); j++) {
				m_layers[i][j].Export(jsonOb[to_string(i)][to_string(j)]);
			}
		}
		FileSystem::SaveJsonToFile(jsonOb, "neuralNetwork");
	}

	void Import() {
		try {
			nlohmann::json jsonOb = FileSystem::LoadJsonFromFile("neuralNetwork");
			for (int i = 0; i < m_layers.size() - 1; i++) {
				for (int j = 0; j < m_layers[i].size(); j++) {
					m_layers[i][j].Import(jsonOb[to_string(i)][to_string(j)]);
				}
			}
		} catch (...) {}
	}
	void feedForward(const vector<double>& inputValues) {
		if (inputValues.size() != m_layers[0].size() - 1) LOG_ERROR("Inconsistent input values!");
		for (uint i = 0; i < inputValues.size(); ++i) {
			m_layers[0][i].setOutputValue(inputValues[i]);
		}

		for (uint layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
			vector<Neuron>& previousLayer = m_layers[layerNum - 1];
			for (uint n = 0; n < m_layers[layerNum].size() - 1; ++n) {
				m_layers[layerNum][n].feedForward(previousLayer);
			}
		}
	}
	void backProp(const vector<double>& targetValues) {
		vector<Neuron>& outputLayer = m_layers.back();
		m_error = 0.0;

		for (uint n = 0; n < outputLayer.size() - 1; ++n) {
			double delta = targetValues[n] - outputLayer[n].getOutputValue();
			m_error += delta * delta;
		}

		m_error /= outputLayer.size() - 1;
		m_error = sqrt(m_error);

		m_recentAverageError = (m_recentAverageError * m_recentAverageSmoothingFactor + m_error) / (m_recentAverageSmoothingFactor + 1.0);

		for (uint n = 0; n < outputLayer.size() - 1; ++n) {
			outputLayer[n].calcOutputGradients(targetValues[n]);
		}

		for (uint layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
			vector<Neuron>& hiddenLayer = m_layers[layerNum];
			vector<Neuron>& nextLayer = m_layers[layerNum + 1];
			for (uint n = 0; n < hiddenLayer.size(); ++n) {
				hiddenLayer[n].calcHiddenGradients(nextLayer);
			}
		}

		for (uint layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
			vector<Neuron>& layer = m_layers[layerNum];
			vector<Neuron>& previousLayer = m_layers[layerNum - 1];

			for (uint n = 0; n < layer.size() - 1; ++n) {
				layer[n].updateInputWeights(previousLayer);
			}
		}
	}
	void getResults(vector<double>& resultValues) const {
		resultValues.clear();
		for (uint n = 0; n < m_layers.back().size() - 1; ++n) {
			resultValues.push_back(m_layers.back()[n].getOutputValue());
		}
	}

	void Draw(AssetRef<Shader>& shader, bool renderType) {
		for (auto& layer : m_layers) {
			for (int i = 0; i < layer.size() - 1; i++) {
				layer[i].Draw(shader, renderType);
			}
		}
	}

private:
	vector<vector<Neuron>> m_layers;
	double m_error;
	double m_recentAverageError;
	double m_recentAverageSmoothingFactor;
};

static vector<vector<double>> outputValues;

struct Pixel {
	AssetRef<Entity> m_entity;
	bool m_enabled;
};
class MenuState : public State {
private:
	String m_name = "Menu";
	AssetRef<Shader> m_shader;
	vector<Topology> topology = { {64, 8}, {64, 8}, {64, 8}, {3, 1} };
	NeuralNetwork* m_network;
	bool renderType = false;
	vector<Pointlight> m_pointLights;
	vector<Pixel> m_cubes;
	GroundRaycast* cast;
	Vector3 m_rayCastPos;
	bool down = false;
	bool downState;
	vector<double> m_inputValues;

public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		for (int i = 0; i < 64; i++) m_inputValues.push_back(1.0);
		drawableLines = new DrawableLine*[128];
		for (int i = 0; i < 128; i++) {
			drawableLines[i] = new DrawableLine();
		}
		cast = new GroundRaycast();
		int sizeX = 8;
		int sizeY = 8;
		loop(y, sizeY) {
			loop(x, sizeX) {
				Entity* e = NEW(Entity(NEW(Model(GetAssetManager()->Get<Model>("Box")->GetMeshes()[0]->Copy()))));
				BasicMaterial* mat = GetMaterialManager()->CreateUnsafe<BasicMaterial>();
				mat->SetAlbedo(GetTextureManager()->GetWhiteTexture());
				mat->SetEmission(GetTextureManager()->GetWhiteTexture());
				mat->m_emissionStrength = 1;
				e->m_position = Vector3(x - ((float)sizeX / 2), y - ((float)sizeY / 2), 0);
				e->m_scale = Vector3(0.50f);
				e->m_model->SetMaterial(mat);
				m_cubes.push_back({ e, true });
			}
		}
		m_pointLights.push_back(Pointlight(Vector3(-10, 5, -5), 100, Color::White() * 10));
		m_pointLights.push_back(Pointlight(Vector3(10, 5, -5), 100, Color::White() * 10));
		m_pointLights.push_back(Pointlight(Vector3(0, 0, -10), 100, Color::White() * 10));
		m_shader = GetShaderManager()->Get("Geometry");

		m_network = NEW(NeuralNetwork(topology));

		GetCamera()->m_position = Vector3(1.0f, 1.5f, 3.5f);
		GetCamera()->m_rotation = Vector3(0.0f, Math::TWO_PI, 0.0f);
		GetPipeline()->m_directionalLight.m_direction = Vector3(Math::HALF_PI, 0.0f, 0.0f);
	}
	void Update(const TimeStep& time) override {

		//if (KeyJustDown(VK_0)) BackPropNumber(m_network, 0);
		//if (KeyJustDown(VK_1)) BackPropNumber(m_network, 1);
		//if (KeyJustDown(VK_2)) BackPropNumber(m_network, 2);
		//if (KeyJustDown(VK_3)) BackPropNumber(m_network, 3);
		//if (KeyJustDown(VK_4)) BackPropNumber(m_network, 4);
		//if (KeyJustDown(VK_5)) BackPropNumber(m_network, 5);
		//if (KeyJustDown(VK_6)) BackPropNumber(m_network, 6);
		//if (KeyJustDown(VK_7)) BackPropNumber(m_network, 7);
		//if (KeyJustDown(VK_8)) BackPropNumber(m_network, 8);
		//if (KeyJustDown(VK_9)) BackPropNumber(m_network, 9);

		if (KeyJustDown(VK_1)) BackPropTriangle(m_network);
		if (KeyJustDown(VK_2)) BackPropCross(m_network);
		if (KeyJustDown(VK_3)) BackPropCircle(m_network);

		GetCamera()->Update(time);
		cast->Get(GetCamera());
		m_rayCastPos = cast->GetZ(-0.75f);

		if (ButtonDown(VK_MOUSE_LEFT)) {
			if (Math::Within(m_rayCastPos.x, -4.5f, 3.5f)) {
				if (Math::Within(m_rayCastPos.y, -4.5f, 3.5f)) {
					int posX = (m_rayCastPos.x + 0.5f - (m_rayCastPos.x < 0 ? 1 : 0));
					int posY = (m_rayCastPos.y + 0.5f - (m_rayCastPos.y < 0 ? 1 : 0));
					int index = ((posY + 4) * 8) + (posX + 4);
					if (Math::Within(index, 0, (int)m_cubes.size())) {
						Pixel& pixel = m_cubes[index];
						BasicMaterial* mat = (BasicMaterial*)pixel.m_entity->m_model->GetMeshes()[0]->GetMaterial().Get();
						if (!down) downState = !pixel.m_enabled;
						pixel.m_enabled = downState;
						down = true;
						if (pixel.m_enabled) {
							mat->m_color = Color::White();
							m_inputValues[index] = 1.0;
						} else {
							mat->m_color = Color::Black();
							m_inputValues[index] = 0.0;
						}
						m_network->feedForward(m_inputValues);
					}
				}
			}
		} else down = false;
		if (ButtonDown(VK_MOUSE_MIDDLE)) {
			for (Pixel& pixel : m_cubes) {
				BasicMaterial* mat = (BasicMaterial*)pixel.m_entity->m_model->GetMeshes()[0]->GetMaterial().Get();
				mat->m_color = Color::White();
				pixel.m_enabled = true;
				for (int i = 0; i < 64; i++) m_inputValues[i] = 1.0;
			}
			m_network->feedForward(m_inputValues);
		}
		for (int i = 0; i < 128; i++) {
			if (drawableLines[i]->enabled) {
				DrawableLine* line = drawableLines[i];
				line->duration -= time.GetMills();
				if (line->duration <= 0) {
					line->enabled = false;
				}
			}
		}
	}
	void RenderGeometry() override {
		m_shader->Bind();
		m_shader->Set("_Iridescence", 4);
		m_shader->Set("_IridescenceStrength", 0);
		m_shader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
		m_shader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
		m_shader->Set("_TransformationMatrix", Matrix4::Identity());
		m_shader->Set("_CameraPosition", GetCamera()->m_position);
		m_shader->Set("_TessellationFactor", (float)0);
		m_shader->Set("_TessellationSlope", (float)0);
		m_shader->Set("_TessellationShift", (float)0);
		m_shader->Set("_TessellationAlpha", (float)0);
		m_network->Draw(m_shader, renderType);
		for (auto& cube : m_cubes) cube.m_entity->Draw(m_shader, GL_PATCHES);
		for (auto& light : m_pointLights) GetPointlightRenderer()->Submit(light);

		if (Math::Within(m_rayCastPos.x, -4.5f, 3.5f)) {
			if (Math::Within(m_rayCastPos.y, -4.5f, 3.5f)) {
				int posX = (m_rayCastPos.x + 0.5f - (m_rayCastPos.x < 0 ? 1 : 0));
				int posY = (m_rayCastPos.y + 0.5f - (m_rayCastPos.y < 0 ? 1 : 0));
				GetLineRenderer()->DrawRectZ(Rect(posX, posY, 1, 1), Color::Red(), -0.76f);
			}
		}

	}
	void RenderUI() override {}
	int a = 0;
	int b = 0;
	void OnImGUI() override {
		ImGui::Checkbox("Show connection weights", &renderType);

		vector<double> results;
		m_network->getResults(results);
		auto it = max_element(results.begin(), results.end());
		int index = distance(results.begin(), it);
		String s = "";
		if (index == 0) s = "Triangle";
		else if (index == 1) s = "Cross";
		else if (index == 2) s = "Circle";
		ImGui::LabelText("Expected shape:", "%s", s.c_str());

		if (ImGui::Button("This is a triangle!")) {
			BackPropTriangle(m_network);
		}

		if (ImGui::Button("This is a cross!")) {
			BackPropCross(m_network);
		}

		if (ImGui::Button("This is a circle!")) {
			BackPropCircle(m_network);
		}

		if (ImGui::Button("Export")) {
			m_network->Export();
		}
		if (ImGui::Button("Import")) {
			m_network->Import();
		}
		//if (ImGui::Button("This is a 0!")) {
		//	BackPropNumber(m_network, 0);
		//}
		//if (ImGui::Button("This is a 1!")) {
		//	BackPropNumber(m_network, 1);
		//}
		//if (ImGui::Button("This is a 2!")) {
		//	BackPropNumber(m_network, 2);
		//}
		//if (ImGui::Button("This is a 3!")) {
		//	BackPropNumber(m_network, 3);
		//}
		//if (ImGui::Button("This is a 4!")) {
		//	BackPropNumber(m_network, 4);
		//}
		//if (ImGui::Button("This is a 5!")) {
		//	BackPropNumber(m_network, 5);
		//}
		//if (ImGui::Button("This is a 6!")) {
		//	BackPropNumber(m_network, 6);
		//}
		//if (ImGui::Button("This is a 7!")) {
		//	BackPropNumber(m_network, 7);
		//}
		//if (ImGui::Button("This is a 8!")) {
		//	BackPropNumber(m_network, 8);
		//}
		//if (ImGui::Button("This is a 9!")) {
		//	BackPropNumber(m_network, 9);
		//}
		//if (ImGui::Button("Start learning")) {
		//	//for (int i = 2000; i >= 0; --i) {
		//	//	int n1 = (int)(2.0 * rand() / double(RAND_MAX));
		//	//	int n2 = (int)(2.0 * rand() / double(RAND_MAX));
		//	//	int t = n1 ^ n2;
		//	//	inputValues.push_back({ (double)n1,(double)n2 });
		//	//	outputValues.push_back({ (double)t });
		//	//}
		//	//
		//	//GetThreadPool()->DoJob([&] {
		//	//	for (int i = 0; i < inputValues.size() - 100; i++) {
		//	//		m_network->feedForward(inputValues[i]);
		//	//		vector<double> outputs;
		//	//		m_network->getResults(outputs);
		//	//		m_network->backProp(outputValues[i]);
		//	//		Sleep(2);
		//	//	}});
		//}
		//if (ImGui::SliderInt("A value", &a, 0, 1)) {
		//	m_network->feedForward({ (double)a, (double)b });
		//
		//}
		//if (ImGui::SliderInt("B value", &b, 0, 1)) {
		//	m_network->feedForward({ (double)a, (double)b });
		//
		//}
		//vector<double> outputs;
		//m_network->getResults(outputs);
		//ImGui::LabelText("Output", "%f", outputs[0]);
		//ImGui::LabelText("Expected output", "%d", a ^ b);
		//ImGui::LabelText("Correct answer?", "%s", (Math::Round(outputs[0]) == a ^ b ? "Yes!" : "No..."));

		for (int i = 0; i < 128; i++) {
			if (drawableLines[i]->enabled) {
				DrawableLine* line = drawableLines[i];
				Color color = Color(line->amount * line->duration / 1000);
				color.A = 1.0;
				GetLineRenderer()->Submit(line->x1, line->x2, color);
			}
		}
	}

	//void BackPropNumber(NeuralNetwork* net, int num) {
	//	vector<double> result = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	//	result[num] = 1.0;
	//	net->backProp(result);
	//}

	void BackPropTriangle(NeuralNetwork* net) {
		vector<double> result = { 0, 0, 1 };
		net->backProp(result);
	}

	void BackPropCross(NeuralNetwork* net) {
		vector<double> result = { 0, 1, 0 };
		net->backProp(result);
	}

	void BackPropCircle(NeuralNetwork* net) {
		vector<double> result = { 1, 0, 1 };
		net->backProp(result);
	}

	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};