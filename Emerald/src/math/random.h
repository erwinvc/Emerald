#pragma once

class RandomInstance {
private:
	std::mt19937 m_random;
	uniform_int_distribution<mt19937::result_type> m_distribution;

public:
	RandomInstance() { m_random.seed(random_device()()); }
	RandomInstance(unsigned int seed) { m_random.seed(seed); }

	float Float(float min, float max) {
		float r = (float)m_distribution(m_random) / (float)numeric_limits<uint32_t>::max();
		return r * (max - min) + min;
	}

	float Float(float max) {
		float r = (float)m_distribution(m_random) / (float)numeric_limits<uint32_t>::max();
		return r * max;
	}

	/// <summary> Generates a random integer </summary>
/// <param name="min"> Inclusive minimum </param>
/// <param name="max"> Inclusive maxmimum </param>
	inline int Int(int min, int max) {
		return min + rand() % ((max + 1) - min);
	}
};

class Random {
private:
	static RandomInstance m_instance;

public:
	static void Initialize() {
		m_instance = RandomInstance((uint32)time(nullptr));
	}
	static float Float(float min, float max) {
		return m_instance.Float(min, max);
	}

	static float Float(float max) {
		return m_instance.Float(max);
	}

	/// <summary> Generates a random integer </summary>
	/// <param name="min"> Inclusive minimum </param>
	/// <param name="max"> Inclusive maxmimum </param>
	static int Int(int min, int max) {
		return m_instance.Int(min, max);
	}
};