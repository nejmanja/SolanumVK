#pragma once

#include "IPipeline.h"

class ComputePipeline : public IPipeline
{
public:
    ComputePipeline(VkDevice device);
    ~ComputePipeline() override;
    void execute(VkCommandBuffer cmd) override;

private:
    void bind(VkCommandBuffer cmd) override;
};