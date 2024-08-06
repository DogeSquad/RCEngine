#pragma once

// Design inspired by Brendan Galea's YouTube tutorials.

#include "rce_window.hpp"
#include "rce_device.hpp"
#include "rce_model.hpp"
#include "rce_object.hpp"
#include "rce_renderer.hpp"

#include <memory>
#include <vector>

namespace rce {
	class RCEngine {
	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 800;

		RCEngine();
		~RCEngine();

		RCEngine(const RCEngine&) = delete;
		RCEngine& operator=(const RCEngine&) = delete;

		void run();
	private:
		void loadObjects();

		RCEWindow rceWindow{ WIDTH, HEIGHT, "Osprey RCE" };
		RCEDevice rceDevice{ rceWindow };
		RCERenderer rceRenderer{ rceWindow, rceDevice };

		std::vector<RCEObject> objects;
	};
}