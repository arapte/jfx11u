/*
 * Copyright (C) 2013-2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "JSBigInt.h"
#include "JSCJSValue.h"
#include "MathCommon.h"
#include "TypedArrayAdaptersForwardDeclarations.h"
#include "TypedArrayType.h"
#include <wtf/MathExtras.h>

namespace JSC {

template<
    typename TypeArg, typename ViewTypeArg, typename JSViewTypeArg,
    TypedArrayType typeValueArg>
struct IntegralTypedArrayAdaptor {
    typedef TypeArg Type;
    typedef ViewTypeArg ViewType;
    typedef JSViewTypeArg JSViewType;
    static constexpr TypedArrayType typeValue = typeValueArg;
    static constexpr TypeArg minValue = std::numeric_limits<TypeArg>::lowest();
    static constexpr TypeArg maxValue = std::numeric_limits<TypeArg>::max();
    static constexpr bool canConvertToJSQuickly = true;
    static constexpr TypedArrayContentType contentType = JSC::contentType(typeValue);

    static JSValue toJSValue(JSGlobalObject*, Type value)
    {
        static_assert(!std::is_floating_point<Type>::value, "");
        return jsNumber(value);
    }

    static Type toNativeFromInt32(int32_t value)
    {
        return static_cast<Type>(value);
    }

    static Type toNativeFromUint32(uint32_t value)
    {
        return static_cast<Type>(value);
    }

    static Type toNativeFromDouble(double value)
    {
        int32_t result = static_cast<int32_t>(value);
        if (static_cast<double>(result) != value)
            result = toInt32(value);
        return static_cast<Type>(result);
    }

    template<typename OtherAdaptor>
    static typename OtherAdaptor::Type convertTo(Type value)
    {
        if (typeValue == TypeUint32)
            return OtherAdaptor::toNativeFromUint32(value);
        return OtherAdaptor::toNativeFromInt32(value);
    }

    static std::optional<Type> toNativeFromInt32WithoutCoercion(int32_t value)
    {
        if ((value >= 0 && static_cast<uint32_t>(value) > static_cast<uint32_t>(maxValue)) || value < static_cast<int32_t>(minValue))
            return std::nullopt;
        return static_cast<Type>(value);
    }

    static std::optional<Type> toNativeFromUint32WithoutCoercion(uint32_t value)
    {
        if (value > static_cast<uint32_t>(maxValue))
            return std::nullopt;

        return static_cast<Type>(value);
    }

    static std::optional<Type> toNativeFromDoubleWithoutCoercion(double value)
    {
        Type integer = static_cast<Type>(value);
        if (static_cast<double>(integer) != value)
            return std::nullopt;

        if (value < 0)
            return toNativeFromInt32WithoutCoercion(static_cast<int32_t>(value));

        return toNativeFromUint32WithoutCoercion(static_cast<uint32_t>(value));
    }
};

template<
    typename TypeArg, typename ViewTypeArg, typename JSViewTypeArg,
    TypedArrayType typeValueArg>
struct FloatTypedArrayAdaptor {
    typedef TypeArg Type;
    typedef ViewTypeArg ViewType;
    typedef JSViewTypeArg JSViewType;
    static constexpr TypedArrayType typeValue = typeValueArg;
    static constexpr TypeArg minValue = std::numeric_limits<TypeArg>::lowest();
    static constexpr TypeArg maxValue = std::numeric_limits<TypeArg>::max();
    static constexpr bool canConvertToJSQuickly = true;
    static constexpr TypedArrayContentType contentType = JSC::contentType(typeValue);

    static JSValue toJSValue(JSGlobalObject*, Type value)
    {
        return jsDoubleNumber(purifyNaN(value));
    }

    static Type toNativeFromInt32(int32_t value)
    {
        return static_cast<Type>(value);
    }

    static Type toNativeFromUint32(uint32_t value)
    {
        return static_cast<Type>(value);
    }

    static Type toNativeFromDouble(double value)
    {
        return static_cast<Type>(value);
    }

    template<typename OtherAdaptor>
    static typename OtherAdaptor::Type convertTo(Type value)
    {
        return OtherAdaptor::toNativeFromDouble(value);
    }

    static std::optional<Type> toNativeFromInt32WithoutCoercion(int32_t value)
    {
        return static_cast<Type>(value);
    }

    static std::optional<Type> toNativeFromDoubleWithoutCoercion(double value)
    {
        if (std::isnan(value) || std::isinf(value))
            return static_cast<Type>(value);

        Type valueResult = static_cast<Type>(value);

        if (static_cast<double>(valueResult) != value)
            return std::nullopt;

        if (value < minValue || value > maxValue)
            return std::nullopt;

        return valueResult;
    }
};

template<
    typename TypeArg, typename ViewTypeArg, typename JSViewTypeArg,
    TypedArrayType typeValueArg>
struct BigIntTypedArrayAdaptor {
    typedef TypeArg Type;
    typedef ViewTypeArg ViewType;
    typedef JSViewTypeArg JSViewType;
    static constexpr TypedArrayType typeValue = typeValueArg;
    static constexpr TypeArg minValue = std::numeric_limits<TypeArg>::lowest();
    static constexpr TypeArg maxValue = std::numeric_limits<TypeArg>::max();
    static constexpr bool canConvertToJSQuickly = false;
    static constexpr TypedArrayContentType contentType = JSC::contentType(typeValue);

    static JSValue toJSValue(JSGlobalObject* globalObject, Type value)
    {
        ASSERT(globalObject);
        return JSBigInt::makeHeapBigIntOrBigInt32(globalObject, value);
    }

    static Type toNativeFromInt32(int32_t value)
    {
        return static_cast<Type>(value);
    }

    static Type toNativeFromUint32(uint32_t value)
    {
        return static_cast<Type>(value);
    }

    static Type toNativeFromDouble(double value)
    {
        return static_cast<Type>(value);
    }

    template<typename OtherAdaptor>
    static typename OtherAdaptor::Type convertTo(Type value)
    {
        return static_cast<typename OtherAdaptor::Type>(value);
    }
};

template<typename Adaptor> class JSGenericTypedArrayView;
using JSInt8Array = JSGenericTypedArrayView<Int8Adaptor>;
using JSInt16Array = JSGenericTypedArrayView<Int16Adaptor>;
using JSInt32Array = JSGenericTypedArrayView<Int32Adaptor>;
using JSUint8Array = JSGenericTypedArrayView<Uint8Adaptor>;
using JSUint8ClampedArray = JSGenericTypedArrayView<Uint8ClampedAdaptor>;
using JSUint16Array = JSGenericTypedArrayView<Uint16Adaptor>;
using JSUint32Array = JSGenericTypedArrayView<Uint32Adaptor>;
using JSFloat32Array = JSGenericTypedArrayView<Float32Adaptor>;
using JSFloat64Array = JSGenericTypedArrayView<Float64Adaptor>;
using JSBigInt64Array = JSGenericTypedArrayView<BigInt64Adaptor>;
using JSBigUint64Array = JSGenericTypedArrayView<BigUint64Adaptor>;

struct Int8Adaptor : IntegralTypedArrayAdaptor<int8_t, Int8Array, JSInt8Array, TypeInt8> { };
struct Int16Adaptor : IntegralTypedArrayAdaptor<int16_t, Int16Array, JSInt16Array, TypeInt16> { };
struct Int32Adaptor : IntegralTypedArrayAdaptor<int32_t, Int32Array, JSInt32Array, TypeInt32> { };
struct Uint8Adaptor : IntegralTypedArrayAdaptor<uint8_t, Uint8Array, JSUint8Array, TypeUint8> { };
struct Uint16Adaptor : IntegralTypedArrayAdaptor<uint16_t, Uint16Array, JSUint16Array, TypeUint16> { };
struct Uint32Adaptor : IntegralTypedArrayAdaptor<uint32_t, Uint32Array, JSUint32Array, TypeUint32> { };
struct Float32Adaptor : FloatTypedArrayAdaptor<float, Float32Array, JSFloat32Array, TypeFloat32> { };
struct Float64Adaptor : FloatTypedArrayAdaptor<double, Float64Array, JSFloat64Array, TypeFloat64> { };
struct BigInt64Adaptor : BigIntTypedArrayAdaptor<int64_t, BigInt64Array, JSBigInt64Array, TypeBigInt64> { };
struct BigUint64Adaptor : BigIntTypedArrayAdaptor<uint64_t, BigUint64Array, JSBigUint64Array, TypeBigUint64> { };

struct Uint8ClampedAdaptor {
    typedef uint8_t Type;
    typedef Uint8ClampedArray ViewType;
    typedef JSUint8ClampedArray JSViewType;
    static constexpr TypedArrayType typeValue = TypeUint8Clamped;
    static constexpr uint8_t minValue = std::numeric_limits<uint8_t>::lowest();
    static constexpr uint8_t maxValue = std::numeric_limits<uint8_t>::max();
    static constexpr bool canConvertToJSQuickly = true;
    static constexpr TypedArrayContentType contentType = JSC::contentType(typeValue);

    static JSValue toJSValue(JSGlobalObject*, uint8_t value)
    {
        return jsNumber(value);
    }

    static Type toNativeFromInt32(int32_t value)
    {
        return clamp(value);
    }

    static Type toNativeFromUint32(uint32_t value)
    {
        return std::min(static_cast<uint32_t>(255), value);
    }

    static Type toNativeFromDouble(double value)
    {
        if (std::isnan(value) || value < 0)
            return 0;
        if (value > 255)
            return 255;
        return static_cast<uint8_t>(lrint(value));
    }

    template<typename OtherAdaptor>
    static typename OtherAdaptor::Type convertTo(uint8_t value)
    {
        return OtherAdaptor::toNativeFromInt32(value);
    }

    static std::optional<Type> toNativeFromInt32WithoutCoercion(int32_t value)
    {
        if (value > maxValue || value < minValue)
            return std::nullopt;

        return static_cast<Type>(value);
    }

    static std::optional<Type> toNativeFromDoubleWithoutCoercion(double value)
    {
        uint8_t integer = static_cast<uint8_t>(value);
        if (static_cast<double>(integer) != value)
            return std::nullopt;

        return integer;
    }

private:
    static uint8_t clamp(int32_t value)
    {
        if (value < 0)
            return 0;
        if (value > 255)
            return 255;
        return static_cast<uint8_t>(value);
    }
};

} // namespace JSC
