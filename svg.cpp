#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator <<(std::ostream& out, Color& color) {
        std::visit(ColorPrint{out}, color);
        return out;
    }

    std::ostream& operator <<(std::ostream& out, StrokeLineCap line_cap) {
        switch (line_cap) {
            case StrokeLineCap::BUTT:
                out << "butt"sv;
                break;
            case StrokeLineCap::ROUND:
                out << "round"sv;
                break;
            case StrokeLineCap::SQUARE:
                out << "square"sv;
                break;
        }
        return out;
    }

    std::ostream& operator <<(std::ostream& out, StrokeLineJoin line_join) {
        switch (line_join) {
            case StrokeLineJoin::ARCS:
                out << "arcs"sv;
                break;
            case StrokeLineJoin::BEVEL:
                out << "bevel"sv;
                break;
            case StrokeLineJoin::MITER:
                out << "miter"sv;
                break;
            case StrokeLineJoin::MITER_CLIP:
                out << "miter_clip"sv;
                break;
            case StrokeLineJoin::ROUND:
                out << "round"sv;
                break;
        }
        return out;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)  {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)  {
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

    Polyline& Polyline::AddPoint(Point point) {
        peaks_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        int i = 0;
        for (const auto &val : peaks_) {
            if (i == peaks_.size() - 1) {
                out << val.x << "," << val.y;
                break;
            }
            out << val.x << ","sv << val.y << " "sv;
        }
        out << "\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos) {
        reference_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext &contex) const {
        auto& out = contex.out;
        out << "<text";
        RenderAttrs(out);
        out <<" x=\""sv << reference_.x << "\" y=\"" << reference_.y << "\" ";
        out << "dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" ";
        out << "font-size=\"" << font_size_ << "\"";
        if (!font_family_.empty()) {
            out << " font-family=\"" << font_family_ << "\" ";
        }
        if (!font_weight_.empty()) {
            out << "font-weight=\"" << font_weight_ << "\"";
        }
        out << ">" << data_ << "</text>";
    }

    void Document::AddPtr(std::unique_ptr<Object> &&obj) {
        objects_.emplace_back(move(obj));
    }

    void Document::Render(std::ostream &out) const {
        RenderContext context_(out, 2, 2);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
        for (const auto &val : objects_ ) {
            val->Render(context_);
        }
        out << "</svg>\n"sv;
    }
}