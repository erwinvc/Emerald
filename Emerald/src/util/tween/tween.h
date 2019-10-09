#pragma once

enum class Ease {
	LINEAR,
	INSINE,
	OUTSINE,
	INOUTSINE,
	INQUAD,
	OUTQUAD,
	INOUTQUAD,
	INCUBIC,
	OUTCUBIC,
	INOUTCUBIC,
	INQUART,
	OUTQUART,
	INOUTQUART,
	INQUINT,
	OUTQUINT,
	INOUTQUINT,
	INEXPO,
	OUTEXPO,
	INOUTEXPO,
	INCIRC,
	OUTCIRC,
	INOUTCIRC,
	INELASTIC,
	OUTELASTIC,
	INOUTELASTIC,
	INBACK,
	OUTBACK,
	INOUTBACK
};

String_t g_easeStrings[] = {
	"LINEAR",
	"INSINE",
	"OUTSINE",
	"INOUTSINE",
	"INQUAD",
	"OUTQUAD",
	"INOUTQUAD",
	"INCUBIC",
	"OUTCUBIC",
	"INOUTCUBIC",
	"INQUART",
	"OUTQUART",
	"INOUTQUART",
	"INQUINT",
	"OUTQUINT",
	"INOUTQUINT",
	"INEXPO",
	"OUTEXPO",
	"INOUTEXPO",
	"INCIRC",
	"OUTCIRC",
	"INOUTCIRC",
	"INELASTIC",
	"OUTELASTIC",
	"INOUTELASTIC",
	"INBACK",
	"OUTBACK",
	"INOUTBACK"
};

class Tween {
protected:
	Ease m_ease = Ease::LINEAR;
	function<void()> m_onComplete = [] {};
public:
	virtual void Update(const TimeStep& time) = 0;

	template<typename T>
	static AssetRef<Tween> To(T& value, T goal, float duration) {
		//Tween** slot = GetTweenManager()->GetEmptySpot();
		//if (slot == nullptr) LOG_ERROR("[~gTweens~x] no free tween slots available");
		Tween* tween = NEW(TweenValue<T>(&value, goal, duration));
		GetTweenManager()->AddTween(tween);
		return AssetRef<Tween>(tween);
	}

	void SetOnComplete(function<void()> onComplete) { m_onComplete = onComplete; }

	Tween* SetEase(Ease ease) {
		m_ease = ease;
		return this;
	}

	void Kill();

	virtual ~Tween();
};

template<typename T>
class TweenValue : public Tween {
private:
	T* m_value;
	T m_from;
	T m_goal;
	float m_time;
	float m_duration;

	static float Evaluate(Ease ease, float time, float duration) {
		float overshootOrAmplitude = 1;
		float period = 0;
		switch (ease) {
			case Ease::LINEAR: return time / duration;
			case Ease::INSINE: return -(float)Math::Cos(time / duration * Math::HALF_PI) + 1;
			case Ease::OUTSINE: return (float)Math::Sin(time / duration * Math::HALF_PI);
			case Ease::INOUTSINE: return -0.5f * ((float)Math::Cos(Math::PI * time / duration) - 1);
			case Ease::INQUAD: return (time /= duration) * time;
			case Ease::OUTQUAD: return -(time /= duration) * (time - 2);
			case Ease::INOUTQUAD:
				if ((time /= duration * 0.5f) < 1) return 0.5f * time * time;
				return -0.5f * ((--time) * (time - 2) - 1);

			case Ease::INCUBIC:return (time /= duration) * time * time;
			case Ease::OUTCUBIC:return ((time = time / duration - 1) * time * time + 1);
			case Ease::INOUTCUBIC:
				if ((time /= duration * 0.5f) < 1) return 0.5f * time * time * time;
				return 0.5f * ((time -= 2) * time * time + 2);
			case Ease::INQUART:return (time /= duration) * time * time * time;
			case Ease::OUTQUART:return -((time = time / duration - 1) * time * time * time - 1);
			case Ease::INOUTQUART:
				if ((time /= duration * 0.5f) < 1) return 0.5f * time * time * time * time;
				return -0.5f * ((time -= 2) * time * time * time - 2);
			case Ease::INQUINT:return (time /= duration) * time * time * time * time;
			case Ease::OUTQUINT:return ((time = time / duration - 1) * time * time * time * time + 1);
			case Ease::INOUTQUINT:
				if ((time /= duration * 0.5f) < 1) return 0.5f * time * time * time * time * time;
				return 0.5f * ((time -= 2) * time * time * time * time + 2);
			case Ease::INEXPO:return (time == 0) ? 0 : (float)Math::Pow(2, 10 * (time / duration - 1));
			case Ease::OUTEXPO:
				if (time == duration) return 1;
				return (-(float)Math::Pow(2, -10 * time / duration) + 1);
			case Ease::INOUTEXPO:
				if (time == 0) return 0;
				if (time == duration) return 1;
				if ((time /= duration * 0.5f) < 1) return 0.5f * (float)Math::Pow(2, 10 * (time - 1));
				return 0.5f * (-(float)Math::Pow(2, -10 * --time) + 2);
			case Ease::INCIRC:return -((float)Math::Sqrt(1 - (time /= duration) * time) - 1);
			case Ease::OUTCIRC:return (float)Math::Sqrt(1 - (time = time / duration - 1) * time);
			case Ease::INOUTCIRC:
				if ((time /= duration * 0.5f) < 1) return -0.5f * ((float)Math::Sqrt(1 - time * time) - 1);
				return 0.5f * ((float)Math::Sqrt(1 - (time -= 2) * time) + 1);
			case Ease::INELASTIC:
				float s0;
				if (time == 0) return 0;
				if ((time /= duration) == 1) return 1;
				if (period == 0) period = duration * 0.3f;
				if (overshootOrAmplitude < 1) {
					overshootOrAmplitude = 1;
					s0 = period / 4;
				} else s0 = period / Math::TWO_PI * (float)Math::Asin(1 / overshootOrAmplitude);
				return -(overshootOrAmplitude * (float)Math::Pow(2, 10 * (time -= 1)) * (float)Math::Sin((time * duration - s0) * Math::TWO_PI / period));
			case Ease::OUTELASTIC:
				float s1;
				if (time == 0) return 0;
				if ((time /= duration) == 1) return 1;
				if (period == 0) period = duration * 0.3f;
				if (overshootOrAmplitude < 1) {
					overshootOrAmplitude = 1;
					s1 = period / 4;
				} else s1 = period / Math::TWO_PI * (float)Math::Asin(1 / overshootOrAmplitude);
				return (overshootOrAmplitude * (float)Math::Pow(2, -10 * time) * (float)Math::Sin((time * duration - s1) * Math::TWO_PI / period) + 1);
			case Ease::INOUTELASTIC:
				float s;
				if (time == 0) return 0;
				if ((time /= duration * 0.5f) == 2) return 1;
				if (period == 0) period = duration * (0.3f * 1.5f);
				if (overshootOrAmplitude < 1) {
					overshootOrAmplitude = 1;
					s = period / 4;
				} else s = period / Math::TWO_PI * (float)Math::Asin(1 / overshootOrAmplitude);
				if (time < 1) return -0.5f * (overshootOrAmplitude * (float)Math::Pow(2, 10 * (time -= 1)) * (float)Math::Sin((time * duration - s) * Math::TWO_PI / period));
				return overshootOrAmplitude * (float)Math::Pow(2, -10 * (time -= 1)) * (float)Math::Sin((time * duration - s) * Math::TWO_PI / period) * 0.5f + 1;
			case Ease::INBACK:return (time /= duration) * time * ((overshootOrAmplitude + 1) * time - overshootOrAmplitude);
			case Ease::OUTBACK:return ((time = time / duration - 1) * time * ((overshootOrAmplitude + 1) * time + overshootOrAmplitude) + 1);
			case Ease::INOUTBACK:
				if ((time /= duration * 0.5f) < 1) return 0.5f * (time * time * (((overshootOrAmplitude *= (1.525f)) + 1) * time - overshootOrAmplitude));
				return 0.5f * ((time -= 2) * time * (((overshootOrAmplitude *= (1.525f)) + 1) * time + overshootOrAmplitude) + 2);
			default: return time / duration;
		}
	}

public:
	TweenValue(T* value, T& goal, float duration) : m_value(value), m_from(*value), m_goal(goal), m_time(0), m_duration(duration) {}

	void Update(const TimeStep& time) override {
		m_time += time;
		float eval = Evaluate(m_ease, m_time, m_duration);
		*m_value = m_from + (m_goal - m_from) * eval;

		if (m_time >= m_duration) {
			*m_value = m_goal;
			m_onComplete();
			GetTweenManager()->Delete(this);
		}
	}
};


class TweenManager : public Singleton<TweenManager> {
private:
	vector<Tween*> m_tweens;
	//Tween* m_tweens[10] = { nullptr };

	TweenManager() {}
	~TweenManager() {}

	void AddTween(Tween* tween) {
		m_tweens.push_back(tween);
	}

	void Delete(Tween* tween) {
		if (Utils::VectorContains(m_tweens, tween)) {
			Utils::RemoveFromVector(m_tweens, tween);
			delete tween;
		}
	}
	friend Tween;
	template<class T> friend class TweenValue;
	friend Singleton;
public:
	void Update(const TimeStep& time) {
		for (int i = 0; i < m_tweens.size(); i++) {
			m_tweens[i]->Update(time);
		}
	}
};

static TweenManager* GetTweenManager() { return TweenManager::GetInstance(); }