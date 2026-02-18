#pragma once

namespace Iberus {

	class Application;

	/// Creates the main game application. Used by the entry point when not running tests.
	Application* CreateGameApplication();

}
