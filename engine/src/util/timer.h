#pragma once

namespace emerald {
	class Timer {
	public:
		Timer() : m_start(getTime()) {}
		virtual ~Timer() = default;

		float get() const {
			float currentTimeMs = getTime();
			float elapsedTimeMs = currentTimeMs - m_start;
			return elapsedTimeMs;
		}

		void print() const {
			Log::info("{:.2f}ms", get());
		}

		void print(std::string_view name) const {
			Log::info("{} took {:.2f}ms", name, get());
		}

	protected:
		virtual float getTime() const;
	private:
		float m_start;
	};

	class HighPrecisionTimer : public Timer {
	protected:
		float getTime() const override;
	};
}