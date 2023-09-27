#include "svg.h"

std::string VerifySymbol(char ch) {
    std::string result;
    switch (ch)
    {
    case '<': result = "&lt;"; break;
    case '>': result = "&gt;"; break;
    case '&': result = "&amp;"; break;
    case '\"': result = "&quot;"; break;
    case '\'': result = "&apos;"; break;
    default: result = ch;
    }
    return result;
}

//===============================================================================================================================OPERATOR<<
//_______________________________________________________________________________________________________________________________for StrokeLineCap
std::ostream& svg::operator<<(std::ostream& out, const svg::StrokeLineCap& line_cap) {
    using namespace std::literals;
    switch (line_cap) {
    case svg::StrokeLineCap::BUTT:
        out << "butt"sv;
        break;
    case svg::StrokeLineCap::ROUND:
        out << "round"sv;
        break;
    case svg::StrokeLineCap::SQUARE:
        out << "square"sv;
    }
    return out;
}

//_______________________________________________________________________________________________________________________________for StrokeLineJoin
std::ostream& svg::operator<<(std::ostream& out, const svg::StrokeLineJoin& line_join) {
    using namespace std::literals;
    switch (line_join) {
    case svg::StrokeLineJoin::ARCS:
        out << "arcs"sv;
        break;
    case svg::StrokeLineJoin::BEVEL:
        out << "bevel"sv;
        break;
    case svg::StrokeLineJoin::MITER:
        out << "miter"sv;
        break;
    case svg::StrokeLineJoin::MITER_CLIP:
        out << "miter-clip"sv;
        break;
    case svg::StrokeLineJoin::ROUND:
        out << "round"sv;
    }
    return out;
}

//_______________________________________________________________________________________________________________________________for Color
std::ostream& svg::operator<<(std::ostream& out, const svg::Color& color) {
    return std::visit(svg::PrintColor{ out }, color);
}
//_______________________________________________________________________________________________________________________________


//============================================================================================================================================
namespace svg {//================================================================================================================NAMESPACE_SVG

    using namespace std::literals;
    //============================================================================================================================OPERATOR() FOR COLOR VARIANT
    std::ostream& PrintColor::operator()(std::monostate) const {
        out << NoneColor;
        return out;
    }
    std::ostream& PrintColor::operator()(const Rgb& rgb) const {
        out << "rgb("sv << static_cast<int>(rgb.red) << ","sv << static_cast<int>(rgb.green) << ","sv << static_cast<int>(rgb.blue) << ")"sv;
        return out;
    }
    std::ostream& PrintColor::operator()(const Rgba& rgba) const {
        out << "rgba("sv << static_cast<int>(rgba.red) << ","sv << static_cast<int>(rgba.green) << ","sv << static_cast<int>(rgba.blue) << ","sv << rgba.opacity << ")"sv;
        return out;
    }
    std::ostream& PrintColor::operator()(const std::string& color) const {
        out << color;
        return out;
    }
    //___________________________________________________________________________________________________________________________


    //============================================================================================================================RENDER

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        RenderObject(context);

        context.out << std::endl;
    }
    //___________________________________________________________________________________________________________________________


    //============================================================================================================================CIRCLE

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }
    //___________________________________________________________________________________________________________________________


    //============================================================================================================================POLYLINE

    Polyline& Polyline::AddPoint(Point point) {
        polyline_.push_back(point);
        return *this;
    }


    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (auto point = polyline_.begin(); point != polyline_.end(); ++point) {
            out << point->x << ","sv << point->y;
            if (std::next(point) != polyline_.end()) {
                out << " "sv;
            }
        }
        out << "\"";
        RenderAttrs(out);
        out << "/>"sv;
    }
    //___________________________________________________________________________________________________________________________


   //============================================================================================================================TEXT

    Text& Text::SetPosition(Point pos) {
        position_ = std::move(pos);
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = std::move(offset);
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = std::move(size);
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    // “<text ”, через пробел свойства в произвольном порядке, затем символ “>”, содержимое надписи “</text>”. 
    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text x=\"" << position_.x << "\" y=\"" << position_.y << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" ";
        out << "font-size=\"" << font_size_ << "\"";
        RenderAttrs(out);
        if (!font_weight_.empty()) {
            out << " font-weight=\"" << font_weight_ << "\"";
        }
        if (!font_family_.empty()) {
            out << " font-family=\"" << font_family_ << "\"";
        }
        out << ">";
        for (const char ch : data_) {
            out << VerifySymbol(ch);
        }
        out << "</text>";
    }
    //___________________________________________________________________________________________________________________________


   //============================================================================================================================DOCUMENT
    // Добавляет в svg-документ объект-наследник svg::Object
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objects_) {
            obj->Render(RenderContext(out));
        }
        out << "</svg>"sv << std::endl;
    }
    //=======================================================================================================================================
}//=========================================================================================================================NAMESPACE_SVG