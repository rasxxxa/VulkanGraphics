export module Helper;

using ushort = unsigned short;

export
{
	struct SDLWindowSize
	{
		int width = 600;
		int height = 600;
	};

	SDLWindowSize WindowSize;

	bool RequestValidation = true;

	struct VulkanVersion
	{
		ushort major = 1;
		ushort minor = 1;
	} VkVersion;

	constexpr ushort FRAMES = 2;

	constexpr ushort NUM_TEST_OBJECTS = 2000;

	struct Color
	{
		float r;
		float g;
		float b;
		Color() : r(0.0f), g(0.0f), b(0.0f)
		{

		}
		Color(float rr, float gg, float bb) : r(rr), g(gg), b(bb)
		{

		}
	};

	Color Brown(38.0f / 255.0f, 14.0f / 255.0f, 0.0f);
	Color Appricot(111.0 / 255.0f, 111.0f / 255.0f, 0.0f);
}
