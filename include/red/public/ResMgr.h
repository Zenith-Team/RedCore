#include <system/ResMgr.h>

namespace red::pub {
    
    class ResMgr : public ::ResMgr {
    public:
        using ::ResMgr::ResHolder;
        using ::ResMgr::CourseArchiveResHolder;
    
        using ::ResMgr::loadCourseResPackImpl_;
        using ::ResMgr::loadArchiveResImpl_;
        using ::ResMgr::getFileFromArchiveResImpl_;
        
        using ::ResMgr::mCourseArchiveRes;
        using ::ResMgr::mResHolderTreeMap;
        using ::ResMgr::mSZSDecompressor;
    };
    
}
