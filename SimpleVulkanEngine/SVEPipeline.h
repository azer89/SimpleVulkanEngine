#ifndef SVE_PIPELINE_H
#define SVE_PIPELINE_H

#include <string>
#include <vector>

class SVEPipeline
{
public:
	SVEPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

private:
	static std::vector<char> readFile(const std::string& filepath);

	void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
};

#endif