#include <Iberus.h>

class Sandbox : public Iberus::Application {
public:
	Sandbox() {
	}
	~Sandbox() {
	}
};
 
Iberus::Application* Iberus::CreateApplication() {
	return new Sandbox();
}