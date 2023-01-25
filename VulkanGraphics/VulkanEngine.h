class VulkanEngine
{
public:
	VulkanEngine();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	void Init();
	void Run();
	void CleanUp();
};