#pragma once
#include <vector>
#include "Vector.h"

class Path2D {
private:
    struct Segment {
        enum class Type { Linear, CubicBezier };
        Type type;
        Vector2D p0, p1, p2, p3; // 4点でキュービック
        float length;
    };
    std::vector<Segment> segments;
    float totalLength = 0.f;

    static float EstimateBezierLength(const Vector2D& p0, const Vector2D& p1, const Vector2D& p2, const Vector2D& p3, int samples = 10) {
        float len = 0.f;
        Vector2D prev = p0;
        for (int i = 1; i <= samples; ++i) {
            float t = i / (float)samples;
            float u = 1 - t;
            Vector2D point =
                u * u * u * p0 +
                3 * u * u * t * p1 +
                3 * u * t * t * p2 +
                t * t * t * p3;
            len += (point - prev).Length();
            prev = point;
        }
        return len;
    }

public:
    void SetPath(const std::vector<Vector2D>& points, const std::vector<int>& segmentInterpolations) {
        segments.clear();
        totalLength = 0.f;

        for (size_t i = 0; i < segmentInterpolations.size(); ++i) {
            if (segmentInterpolations[i] == 0) {
                // Linear
                if (i + 1 >= points.size()) break;
                Segment seg;
                seg.type = Segment::Type::Linear;
                seg.p0 = points[i];
                seg.p1 = points[i + 1];
                seg.length = (seg.p1 - seg.p0).Length();
                segments.push_back(seg);
                totalLength += seg.length;
            }
            else {
                // Cubic Bezier（自然スプライン風制御点計算）
                if (i + 1 >= points.size()) break;
                Vector2D P0 = points[i];
                Vector2D P3 = points[i + 1];
                Vector2D P_minus1 = (i == 0) ? P0 : points[i - 1];
                Vector2D P_plus2 = (i + 2 >= points.size()) ? P3 : points[i + 2];

                Vector2D P1 = P0 + (P3 - P_minus1) / 6.0;
                Vector2D P2 = P3 - (P_plus2 - P0) / 6.0;

                Segment seg;
                seg.type = Segment::Type::CubicBezier;
                seg.p0 = P0;
                seg.p1 = P1;
                seg.p2 = P2;
                seg.p3 = P3;
                seg.length = EstimateBezierLength(P0, P1, P2, P3);
                segments.push_back(seg);
                totalLength += seg.length;
            }
        }
    }

    Vector2D GetPositionAt(float t) const {
        if (segments.empty()) return Vector2D(0, 0);
        if (t <= 0.f) return segments.front().p0;
        if (t >= 1.f) {
            const auto& seg = segments.back();
            if (seg.type == Segment::Type::Linear) return seg.p1;
            else return seg.p3;
        }

        float target = t * totalLength;
        float accumulated = 0.f;

        for (const auto& seg : segments) {
            if (accumulated + seg.length >= target) {
                float localT = (target - accumulated) / seg.length;
                if (seg.type == Segment::Type::Linear) {
                    return seg.p0 * (1 - localT) + seg.p1 * localT;
                }
                else {
                    float u = 1 - localT;
                    return seg.p0 * (u * u * u) + seg.p1 * (3 * u * u * localT) + seg.p2 * (3 * u * localT * localT) + seg.p3 * (localT * localT * localT);
                }
            }
            accumulated += seg.length;
        }

        // fallback
        const auto& seg = segments.back();
        if (seg.type == Segment::Type::Linear) return seg.p1;
        else return seg.p3;
    }
};
