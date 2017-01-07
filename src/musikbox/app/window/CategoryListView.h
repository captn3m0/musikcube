//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2016 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <sigslot/sigslot.h>

#include <cursespp/ListWindow.h>
#include <cursespp/ScrollAdapterBase.h>

#include <glue/query/CategoryListQuery.h>

#include <core/audio/PlaybackService.h>
#include <core/library/IQuery.h>
#include <core/library/ILibrary.h>
#include <core/runtime/IMessage.h>

#include <mutex>

namespace musik {
    namespace box {
        class CategoryListView :
            public cursespp::ListWindow,
#if (__clang_major__ == 7 && __clang_minor__ == 3)
            public std::enable_shared_from_this<CategoryListView>,
#endif
            public sigslot::has_slots<>
        {
            public:
                CategoryListView(
                    musik::core::audio::PlaybackService& playback,
                    musik::core::LibraryPtr library,
                    const std::string& fieldName);

                virtual ~CategoryListView();

                void RequeryWithField(
                    const std::string& fieldName,
                    const std::string& filter = "",
                    const DBID selectAfterQuery = 0);

                void Requery(
                    const std::string& filter = "",
                    const DBID selectAfterQuery = 0);

                virtual bool KeyPress(const std::string& key);

                void Reset();

                virtual void ProcessMessage(musik::core::runtime::IMessage &message);

                DBID GetSelectedId();
                std::string GetFieldName();
                void SetFieldName(const std::string& fieldName);

            protected:
                virtual cursespp::IScrollAdapter& GetScrollAdapter();
                void OnQueryCompleted(musik::core::IQueryPtr query);

                class Adapter : public cursespp::ScrollAdapterBase {
                public:
                    Adapter(CategoryListView &parent);

                    virtual size_t GetEntryCount();

                    virtual cursespp::IScrollAdapter::EntryPtr
                        GetEntry(cursespp::ScrollableWindow* window, size_t index);

                private:
                    CategoryListView &parent;
                    cursespp::IScrollAdapter::ScrollPosition spos;
                };

            private:
                void OnTrackChanged(size_t index, musik::core::TrackPtr track);
                void ScrollToPlaying();

                musik::core::audio::PlaybackService& playback;
                Adapter *adapter;

                std::mutex queryMutex;
                std::shared_ptr<musik::glue::CategoryListQuery> activeQuery;

                musik::core::LibraryPtr library;
                musik::core::TrackPtr playing;

                std::string fieldName;
                DBID selectAfterQuery;
                musik::glue::CategoryListQuery::ResultList metadata;
        };
    }
}
