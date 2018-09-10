#pragma once

#include <memory>
#include <wrl/client.h>

#include "IRenderable.hpp"

#include "PlanetRendererLow.hpp"
#include "PlanetRendererMed.hpp"
#include "PlanetRendererHigh.hpp"

namespace Galactic {
	enum EPlanetDetail { Low, Medium, High };

	std::shared_ptr<Galactic::IRenderable> CreatePlanetRenderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, EPlanetDetail detail)
	{
		std::shared_ptr<IRenderable> renderer;

		switch (detail) {
			case EPlanetDetail::Low:
				renderer = std::make_shared<LowDetailPlanetRenderer>(deviceContext);
				break;

			case EPlanetDetail::Medium:
				renderer = std::make_shared<MedDetailPlanetRenderer>(deviceContext);
				break;

			case EPlanetDetail::High:
				renderer = std::make_shared<HighDetailPlanetRenderer>(deviceContext);
				break;
		}

		return renderer;
	}
}