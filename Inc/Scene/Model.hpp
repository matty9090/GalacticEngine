#include "Render/IRenderable.hpp"
#include "Render/Drawable.hpp"

namespace Galactic
{
	struct ModelVertex
	{

	};

	class Model : public IRenderable, Drawable<ModelVertex>
	{
		public:
			Model(ID3D11DeviceContext *context, std::string file);

			void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
			void Update(float dt);
			void Reset();

		private:
			ID3D11DeviceContext *m_context;
	};
}