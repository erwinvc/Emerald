#pragma once

class Neuron;

struct Connection {
	double weight;
	double deltaWeight;
	Neuron* neuron;
	double passingValue;
};

class Neuron {
public:
	static double randomWeight() { return rand() / double(RAND_MAX); }

	Neuron(uint numOutputs, uint myIndex, float x, float y) : m_myIndex(myIndex) {
		Model* model = NEW(Model(GetAssetManager()->Get<Model>("Sphere")->GetMeshes()[0]->Copy()));
		m_material = GetMaterialManager()->CreateUnsafe<BasicMaterial>();
		m_material->SetAlbedo(GetAssetManager()->Get<Texture>("White"));
		model->SetMaterial(m_material);
		m_entity = NEW(Entity(model));
		m_entity->m_position.x = x;
		m_entity->m_position.y = y;
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
			if (c.deltaWeight < 0) color.B = -value;
			else color.R = value;

			float x2 = Math::Clamp((float)c.deltaWeight, 0.0f, 1.0f);
			GetLineRenderer()->Submit(m_entity->m_position, c.neuron->m_entity->m_position, color);
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
	NeuralNetwork(const vector<uint>& topology) {
		uint numLayers = topology.size();
		for (int layerNum = 0; layerNum < numLayers; ++layerNum) {
			m_layers.push_back(vector<Neuron>());
			uint numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];
			for (int neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
				float xPos = neuronNum - ((float)topology[layerNum] / 2);
				m_layers[layerNum].push_back(Neuron(numOutputs, neuronNum, xPos, layerNum));
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

static vector<vector<double>> inputValues;
static vector<vector<double>> outputValues;

class MenuState : public State {
private:
	String m_name = "Menu";
	AssetRef<Shader> m_shader;
	vector<uint> topology = { 2, 3, 8, 4, 1 };
	NeuralNetwork* m_network;
	bool renderType = false;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {
		m_shader = GetShaderManager()->Get("Geometry");

		m_network = NEW(NeuralNetwork(topology));

		GetCamera()->m_position = Vector3(1.0f, 1.5f, 3.5f);
		GetCamera()->m_rotation = Vector3(0.0f, Math::TWO_PI, 0.0f);
		GetPipeline()->m_directionalLight.m_direction = Vector3(Math::HALF_PI, 0.0f, 0.0f);
	}
	void Update(const TimeStep& time) override {
		GetCamera()->Update(time);
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
	}
	void RenderUI() override {}
	int a = 0;
	int b = 0;
	void OnImGUI() override {
		if (ImGui::Button("Start learning")) {
			for (int i = 2000; i >= 0; --i) {
				int n1 = (int)(2.0 * rand() / double(RAND_MAX));
				int n2 = (int)(2.0 * rand() / double(RAND_MAX));
				int t = n1 ^ n2;
				inputValues.push_back({ (double)n1,(double)n2 });
				outputValues.push_back({ (double)t });
			}

			GetThreadPool()->DoJob([&] {
				for (int i = 0; i < inputValues.size() - 100; i++) {
					m_network->feedForward(inputValues[i]);
					vector<double> outputs;
					m_network->getResults(outputs);
					m_network->backProp(outputValues[i]);
					Sleep(5);
				}});
		}
		ImGui::Checkbox("Show connection weights", &renderType);
		if (ImGui::SliderInt("A value", &a, 0, 1)) {
			m_network->feedForward({ (double)a, (double)b });

		}
		if (ImGui::SliderInt("B value", &b, 0, 1)) {
			m_network->feedForward({ (double)a, (double)b });

		}
		vector<double> outputs;
		m_network->getResults(outputs);
		ImGui::LabelText("Output", "%f", outputs[0]);
		ImGui::LabelText("Expected output", "%d", a ^ b);
		ImGui::LabelText("Correct answer?", "%s", (Math::Round(outputs[0]) == a ^ b ? "Yes!" : "No..."));
	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};