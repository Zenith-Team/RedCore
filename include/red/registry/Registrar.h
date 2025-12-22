#pragma once

#include <red/registry/builder/ProfileCreateBuilder.h>
#include <concepts>

namespace red {

class Registrar {
public:
    Registrar(const char* nameSpace)
        : mNamespace(nameSpace)
    { }

    /*
        One profile per instanciation of T, meaning if you want two profiles with the same class, you
        gotta make a second class that inherits the first to force creation of a new template instance.
    */
    template <class T> requires std::derived_from<T, Actor>
    ProfileCreateBuilder profile(const char* name) const {
        static char buf[ProfileEx::cNameMaxLen + 1];
        static sead::BufferedSafeString sIdentifier{buf, cNameMaxLen};
        sIdentifier.format("%s:%s", mNamespace.cstr(), mName);
        
        return ProfileCreateBuilder<T>(&sIdentifier);
    }
    
    template <class T> requires std::derived_from<T, Actor>
    ProfileReplaceBuilder profile(s32 id) const {
        return ProfileReplaceBuilder<T>(id);
    }
    
    ProfileEditBuilder editProfile(s32 id) const {
        return ProfileEditBuilder(id);
    }

private:
    const char* mNamespace;
};

}
