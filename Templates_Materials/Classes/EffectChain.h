/*
  ==============================================================================

    EffectChain.h
    Created: 11 Mar 2026 1:38:24pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "MonoEffect.h"
#include <functional>

class MonoEffectChainLink {
public:
    
    virtual ~MonoEffectChainLink() = default;
    
    MonoEffectChainLink(MonoEffect* _effect) {
        next   = nullptr;
        effect = _effect;
    }
    
    virtual void setNext(MonoEffectChainLink* _next) { next = _next; }
    void setEffect(MonoEffect* _effect) { effect = _effect; }
    
    virtual MonoEffectChainLink* getNext() { return next; }
    MonoEffect* getEffect() { return effect; }
    
    void prepare(float fs, int blockSize) {
        effect->prepare(fs, blockSize);
    }
    
    virtual float processSample(float xn) {
        return effect->processSample(xn);
    }
    
protected:
    MonoEffectChainLink* next;
    MonoEffect* effect;
};


template <size_t numChains>
class MonoEffectChain : public MonoEffect {
public:
    
    MonoEffectChain() {
        firsts.fill(nullptr);
        lasts.fill(nullptr);
    }
    
    ~MonoEffectChain() {
        for (MonoEffectChainLink* first : firsts) {
            MonoEffectChainLink* ptr = first;
            while (ptr) {
                MonoEffectChainLink* nxt = ptr->getNext();
                delete ptr;
                ptr = nxt;
            }
        }
    }
    
    void add_link(MonoEffectChainLink* newLink, size_t chainIdx = 0) {
        chainIdx = std::min(chainIdx, numChains - 1);
        if (!firsts[chainIdx]) {
            firsts[chainIdx] = newLink;
            lasts[chainIdx]  = newLink;
        } else {
            lasts[chainIdx]->setNext(newLink);
            lasts[chainIdx] = newLink;
        }
    }
    
    void appendEffect(size_t chainIdx, MonoEffect* effect) {
        MonoEffectChainLink* newLink = new MonoEffectChainLink(effect);
        add_link(newLink, chainIdx);
    }
    
    void appendEffect(MonoEffect* effect) {
        MonoEffectChainLink* newLink = new MonoEffectChainLink(effect);
        add_link(newLink);
    }
    
    float processSample(float xn) override {
        float yn = 0.f;
        for (MonoEffectChainLink* first : firsts) {
            float wn = xn;
            MonoEffectChainLink* ptr = first;
            while (ptr) {
                wn = ptr->processSample(wn);
                ptr = ptr->getNext();
            }
            yn += wn;
        }
        return yn;
    }
    
    void processBlock(float* buffer, size_t numSamples) {
        for (int sample = 0; sample < numSamples; ++sample) {
            processSample(buffer[sample]);
        }
    }
    
    void prepare(float fs, int blockSize) override {
        for (MonoEffectChainLink* first : firsts) {
            MonoEffectChainLink* ptr = first;
            while (ptr) {
                ptr->prepare(fs, blockSize);
                ptr = ptr->getNext();
            }
        }
    }
    
protected:
    std::array<MonoEffectChainLink*, numChains> firsts;
    std::array<MonoEffectChainLink*, numChains> lasts;
};
