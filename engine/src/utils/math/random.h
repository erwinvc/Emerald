#pragma once

namespace emerald {
	//Multiply-with-carry pseudorandom number generator based on George Marsaglia's work
	//https://en.wikipedia.org/wiki/Multiply-with-carry_pseudorandom_number_generator

	class Random {
	public:
		Random() : m_seed(0) {
			initialize();
		}

		Random(uint32_t seed) : m_seed(seed) {
			initialize();
		}

		void reset(uint32_t seed) {
			m_seed = seed;
			initialize();
		}

		uint32_t getInt(void) {
			uint32_t retVal = rand();
			retVal = retVal & (~0u >> 1); //Limit range to positive numbers
			return(retVal);
		}

		int32_t getIntInRange(int32_t minInclusive, int32_t maxExclusive) {
			return minInclusive + getInt() % (maxExclusive - minInclusive);
		}


		float getFloat() {
			return (getInt() & ((1 << 23) - 1)) * 0.00000011920928955078125f; // Mask lower 23 bits
		}

		float getFloatInRange(float min, float max) {
			return min + (max - min) * getFloat();
		}

		float getVariedFloat(float v) {
			return(v * (getFloat() - 0.5f));
		}

		bool getBool() {
			return (getInt() & 1);
		}

		// Generate a Gaussian (normal) distributed random number using the Box-Muller transform
		float getGaussian(float mean = 0.0f, float variance = 1.0f) {
			if (m_hasSpare) {
				m_hasSpare = false;
				return mean + variance * m_spare;
			}

			float x, y, radiusSquared;
			do {
				x = 2.0f * getFloat() - 1.0f;
				y = 2.0f * getFloat() - 1.0f;
				radiusSquared = x * x + y * y;
			} while (radiusSquared >= 1.0f || radiusSquared == 0.0f);

			radiusSquared = std::sqrt(-2.0f * std::log(radiusSquared) / radiusSquared);
			m_spare = y * radiusSquared;
			m_hasSpare = true;
			return mean + variance * x * radiusSquared;
		}

		float getExponential(float lambda = 1.0f) {
			return -std::log(1.0f - getFloat()) / lambda;
		}

	private:
		void initialize() {
			srand(m_seed);
			for (int i = 0; i < CMWC_CYCLE; i++) m_state[i] = rand32();

			do m_carry = rand32();
			while (m_carry >= CMWC_C_MAX);

			m_cycleIndex = CMWC_CYCLE - 1;
		}

		uint32_t rand32(void) {
			uint32_t result = ::rand();
			return result << 16 | ::rand();
		}

		uint32_t rand() {
			uint64_t const a = 18782;
			uint32_t const m = 0xfffffffe;
			uint64_t t;
			uint32_t x;

			// Cycle the index within the state array
			m_cycleIndex = (m_cycleIndex + 1) & (CMWC_CYCLE - 1);

			// Multiply-with-carry algorithm
			t = a * m_state[m_cycleIndex] + m_carry;
			m_carry = t >> 32;
			x = (uint32_t)(t + m_carry);
			if (x < m_carry) {
				x++;
				m_carry++;
			}
			return m_state[m_cycleIndex] = m - x;
		}


		static constexpr uint64_t CMWC_CYCLE = 4096;
		static constexpr uint32_t CMWC_C_MAX = 809430660;

		uint32_t m_seed;
		uint32_t m_state[CMWC_CYCLE];
		uint32_t m_carry;
		uint32_t m_cycleIndex;

		// Gaussian variables
		float m_spare;
		bool m_hasSpare = false;
	};

	class StaticRandom {
	public:
		static inline uint32_t getInt(void) { return getThreadLocalRandom().getInt(); }
		static inline int32_t getIntInRange(int32_t minInclusive, int32_t maxExclusive) { return getThreadLocalRandom().getIntInRange(minInclusive, maxExclusive); }
		static inline float getFloat() { return getThreadLocalRandom().getFloat(); }
		static inline float getFloatInRange(float min, float max) { return getThreadLocalRandom().getFloatInRange(min, max); }
		static inline float getVariedFloat(float v) { return getThreadLocalRandom().getVariedFloat(v); }
		static inline bool getBool() { return getThreadLocalRandom().getBool(); }
		static inline float getGaussian(float min, float max) { return getThreadLocalRandom().getGaussian(min, max); }
		static inline float getExponential(float lambda = 1.0f) { return getThreadLocalRandom().getExponential(lambda); }
	private:
		static Random& getThreadLocalRandom();
		static Random initializeRandom();
	};
}