#pragma once

#include "json.h"

#include <string>
#include <memory>
#include <exception>

namespace json
{
   class ArrayContext;

   class KeyContext;

   class DictContext;

   class Builder {
   public:
      KeyContext Key(const std::string key);
      Builder& Value(Node::Value value);
      ArrayContext StartArray();
      DictContext StartDict();
      Builder& EndArray();
      Builder& EndDict();
      Node& Build();
   protected:
      void StackProcessing();
   private:
      Node root_;
      std::vector<std::unique_ptr<Node>> nodes_stack_;
   };

   //=======================================ADDITIONAL CLASSES============================

   class ArrayContext : public Builder { //_____________________________________StartArray
   public:
      ArrayContext(Builder& builder) : builder_(builder) {}
      //deleted
      KeyContext Key(const std::string key) = delete;
      Builder& EndDict() = delete;
      Node& Build() = delete;
      //using
      Builder& EndArray();
      DictContext StartDict();
      ArrayContext StartArray();
      ArrayContext Value(Node::Value value);
   private:
      Builder& builder_;
   };

   class KeyContext : public Builder { //_____________________________________Key
   public:
      KeyContext(Builder& builder) : builder_(builder) {}
      //deleted
      KeyContext Key(const std::string key) = delete;
      Builder& EndArray() = delete;
      Builder& EndDict() = delete;
      Node& Build() = delete;
      //using
      DictContext StartDict();
      ArrayContext StartArray();
      DictContext Value(Node::Value value);
   private:
      Builder& builder_;
   };


   class DictContext : public Builder { //_____________________________________StartDict
   public:
      DictContext(Builder& builder) : builder_(builder) {}
      //deleted
      Builder& Value(Node::Value value) = delete;
      ArrayContext StartArray() = delete;
      DictContext StartDict() = delete;
      Builder& EndArray() = delete;
      Node& Build() = delete;
      //using
      Builder& EndDict();
      KeyContext Key(const std::string key);
   private:
      Builder& builder_;
   };
}//__________________________________________________________________________________________________________//namespace json

