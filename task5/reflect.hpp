#pragma once

#include <cstdint>


template <class...>
class Annotate {};

struct /* unspecified (field descriptor) */ {
    using Type = /* ... */;
    using Annotations = Annotate</* ... */>;

    template <template <class...> class AnnotationTemplate>
    static constexpr bool has_annotation_template = /* ... */;
    
    template <class Annotation>
    static constexpr bool has_annotation_class = /* ... */;

    template <template <class...> class AnnotationTemplate>
    using FindAnnotation = /* ... */;
};

template <class T>
struct Describe {
    static constexpr std::size_t num_fields = /* number of fields */
    
    template <std::size_t I>
    using Field = /* field descriptor */;
};
