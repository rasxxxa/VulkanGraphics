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

}