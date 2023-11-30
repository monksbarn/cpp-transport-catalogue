#include "json_builder.h"

using namespace json;

void Builder::StackProcessing() {
   if (!nodes_stack_.empty())
   {
      if (nodes_stack_.back()->IsDict())
      {
         const auto key_position = nodes_stack_.end() - 2;
         std::string key = (*key_position)->AsString();
         std::get<Dict>(nodes_stack_.back()->GetValue()).emplace(std::make_pair(key, root_));
         root_ = std::move(*nodes_stack_.back());
         nodes_stack_.erase(key_position, nodes_stack_.end());
      }
      else if (nodes_stack_.back()->IsArray())
      {
         std::get<Array>(nodes_stack_.back()->GetValue()).push_back(std::move(root_));
         root_ = std::move(*nodes_stack_.back());
         nodes_stack_.pop_back();
      }
      else {
         using namespace std::literals;
         throw std::logic_error("Error in file \""s + __FILE__ + "\", after calling the function \""s + __func__ + "()\""s);
      }
   }
}

DictContext Builder::StartDict()
{
   if (root_.IsNull())
   {
      root_ = Node{ Dict{} };
   }
   else {
      nodes_stack_.emplace_back(std::make_unique<Node>(std::move(root_)));
      root_ = Node(Dict{});
   }
   return DictContext(*this);
}


Builder& Builder::EndDict()
{
   if (!nodes_stack_.empty()) {
      StackProcessing();
   }
   return *this;
}


ArrayContext Builder::StartArray()
{
   if (root_.IsNull())
   {
      root_ = Node{ Array{} };
   }
   else {
      nodes_stack_.emplace_back(std::make_unique<Node>(std::move(root_)));
      root_ = Node(Array{});
   }
   return ArrayContext(*this);
}


Builder& Builder::EndArray()
{
   if (!nodes_stack_.empty())
   {
      StackProcessing();
   }
   return *this;
}


KeyContext Builder::Key(const std::string key)
{
   nodes_stack_.emplace_back(std::make_unique<Node>(std::move(key)));
   return KeyContext(*this);
}


Builder& Builder::Value(Node::Value value)
{
   Node tmp;
   tmp.GetValue() = std::move(value);
   if (root_.IsDict())
   {
      std::string key = nodes_stack_.back()->AsString();
      std::get<Dict>(root_.GetValue()).emplace(std::make_pair(key, tmp));
      nodes_stack_.pop_back();
   }
   else if (root_.IsArray())
   {
      std::get<Array>(root_.GetValue()).emplace_back(std::move(tmp));
   }
   else if (root_.IsNull())
   {
      root_ = std::move(tmp);
   }
   else {
      using namespace std::literals;
      throw std::logic_error("Error in file \""s + __FILE__ + "\", after calling the function \""s + __func__ + "()\""s);
   }
   return *this;
}
Node& Builder::Build()
{
   return root_;
}

//______________________________________________________________________KeyContext
DictContext KeyContext::StartDict() {
   return DictContext(builder_.StartDict());
}

DictContext KeyContext::Value(Node::Value value) {
   return DictContext(builder_.Value(value));
}

ArrayContext KeyContext::StartArray() {
   return ArrayContext(builder_.StartArray());
}


//______________________________________________________________________DictContext
KeyContext DictContext::Key(const std::string key) {
   return KeyContext(builder_.Key(key));
}
Builder& DictContext::EndDict() {
   return builder_.EndDict();
}


//______________________________________________________________________ArrayContext
ArrayContext ArrayContext::StartArray() {
   return ArrayContext(builder_.StartArray());
}
Builder& ArrayContext::EndArray() {
   return builder_.EndArray();
}
DictContext ArrayContext::StartDict() {
   return DictContext(builder_.StartDict());
}
ArrayContext ArrayContext::Value(Node::Value value) {
   return ArrayContext(builder_.Value(value));
}


