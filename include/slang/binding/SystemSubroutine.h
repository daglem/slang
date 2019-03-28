//------------------------------------------------------------------------------
// SystemSubroutine.h
// System-defined subroutine handling.
//
// File is under the MIT license; see LICENSE for details.
//------------------------------------------------------------------------------
#pragma once

#include "slang/binding/ConstantValue.h"
#include "slang/util/SmallVector.h"
#include "slang/util/Util.h"

namespace slang {

class BindContext;
class EvalContext;
class Expression;
class Type;

enum class SystemSubroutineFlags {
    None = 0,
    AllowDataTypeArg = 1,
};
BITMASK_DEFINE_MAX_ELEMENT(SystemSubroutineFlags, AllowDataTypeArg);

class SystemSubroutine {
public:
    virtual ~SystemSubroutine() = default;

    using Args = span<const Expression* const>;

    std::string name;
    bitmask<SystemSubroutineFlags> flags;

    virtual const Type& checkArguments(const BindContext& context, const Args& args) const = 0;
    virtual ConstantValue eval(EvalContext& context, const Args& args) const = 0;

protected:
    SystemSubroutine(std::string name,
                     bitmask<SystemSubroutineFlags> flags = SystemSubroutineFlags::None) :
        name(std::move(name)),
        flags(flags) {}

    string_view kindStr() const;
    static bool checkArgCount(const BindContext& context, bool isMethod, const Args& args,
                              size_t expected);
};

} // namespace slang