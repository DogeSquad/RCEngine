#pragma once

#include "rce_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace rce {

class RCEDescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(RCEDevice& rceDevice) : rceDevice{ rceDevice } {}

        Builder& addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<RCEDescriptorSetLayout> build() const;

    private:
        RCEDevice& rceDevice;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    RCEDescriptorSetLayout(
        RCEDevice& rceDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~RCEDescriptorSetLayout();
    RCEDescriptorSetLayout(const RCEDescriptorSetLayout&) = delete;
    RCEDescriptorSetLayout& operator=(const RCEDescriptorSetLayout&) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:
    RCEDevice& rceDevice;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class RCEDescriptorWriter;
};

class RCEDescriptorPool {
public:
    class Builder {
    public:
        Builder(RCEDevice& rceDevice) : rceDevice{ rceDevice } {}

        Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& setMaxSets(uint32_t count);
        std::unique_ptr<RCEDescriptorPool> build() const;

    private:
        RCEDevice& rceDevice;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    RCEDescriptorPool(
        RCEDevice& rceDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~RCEDescriptorPool();
    RCEDescriptorPool(const RCEDescriptorPool&) = delete;
    RCEDescriptorPool& operator=(const RCEDescriptorPool&) = delete;

    bool allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void resetPool();

private:
    RCEDevice& rceDevice;
    VkDescriptorPool descriptorPool;

    friend class RCEDescriptorWriter;
};

class RCEDescriptorWriter {
public:
    RCEDescriptorWriter(RCEDescriptorSetLayout& setLayout, RCEDescriptorPool& pool);

    RCEDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    RCEDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

private:
    RCEDescriptorSetLayout& setLayout;
    RCEDescriptorPool& pool;
    std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace rce