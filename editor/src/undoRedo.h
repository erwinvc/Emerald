#pragma once
#include <stack>
#include <functional>
#include "ref.h"
#include "entities/entity.h"
#include "scene/sceneManager.h"

namespace emerald {
	class UndoActionBase : public RefCounted{
	protected:
		friend class UndoRedo;

		virtual void execute() = 0;
		virtual void undo() = 0;

	public:
		std::string m_name;

		UndoActionBase(const std::string& name) : m_name(name) {}
	};

	template <typename T>
	class UndoAction : public UndoActionBase {
	private:
		friend class UndoRedo;
		T m_userData;
		std::vector<std::function<void(T&)>> m_doActions;
		std::vector<std::function<void(T&)>> m_undoActions;

		void execute() override {
			for (auto& action : m_doActions) {
				action(m_userData);
			}
		}

		void undo() override {
			for (auto& action : m_undoActions) {
				action(m_userData);
			}
		}

	public:
		UndoAction(const std::string& name) : UndoActionBase(name) {}

		void addDoAction(std::function<void(T&)> action) {
			m_doActions.push_back(action);
		}

		void addDoAction(std::function<void()> action) {
			m_doActions.push_back([=](T&) { action(); });
		}

		void addUndoAction(std::function<void(T&)> action) {
			m_undoActions.push_back(action);
		}

		void addUndoAction(std::function<void()> action) {
			m_undoActions.push_back([=](T&) { action(); });
		}
	};

	template <>
	class UndoAction<void> : public UndoActionBase {
	private:
		friend class UndoRedo;
		std::vector<std::function<void()>> m_doActions;
		std::vector<std::function<void()>> m_undoActions;

		void execute() override {
			for (auto& action : m_doActions) {
				action();
			}
		}

		void undo() override {
			for (auto& action : m_undoActions) {
				action();
			}
		}

	public:
		UndoAction(const std::string& name) : UndoActionBase(name) {}

		void addDoAction(std::function<void()> action) {
			m_doActions.push_back(action);
		}

		void addUndoAction(std::function<void()> action) {
			m_undoActions.push_back(action);
		}
	};

	class UndoRedo {
	public:
		template<typename T>
		static Ref<UndoAction<T>> createAction(const std::string& name) {
			return Ref<UndoAction<T>>::create(name);
		}

		template<typename T>
		static void commitAction(Ref<UndoAction<T>>& action) {
			action->execute();
			undoStack.push(action.template as<UndoActionBase>());
			while (!redoStack.empty()) redoStack.pop();
		}

		static void undo() {
			if (!undoStack.empty()) {
				auto command = std::move(undoStack.top());
				undoStack.pop();
				command->undo();
				redoStack.push(std::move(command));
			}
		}

		static void redo() {
			if (!redoStack.empty()) {
				auto command = std::move(redoStack.top());
				redoStack.pop();
				command->execute();
				undoStack.push(std::move(command));
			}
		}
	private:
		static inline std::stack<Ref<UndoActionBase>> undoStack;
		static inline std::stack<Ref<UndoActionBase>> redoStack;
	};
}