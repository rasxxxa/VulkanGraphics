export module Helper;

using ushort = unsigned short;

export
{
	struct SDLWindowSize
	{
		int width = 800;
		int height = 600;
	};

	SDLWindowSize WindowSize;

	bool RequestValidation = true;

	struct VulkanVersion
	{
		ushort major = 1;
		ushort minor = 3;
	} VkVersion;

	constexpr ushort FRAMES = 2;

}