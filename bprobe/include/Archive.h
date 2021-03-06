/*
 * Copyright (c) 1996-2011 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__
 
using namespace std;

class MappedFile;

namespace Dyninst{
namespace SymtabAPI{

class Symtab;

/**
 * Helps facilitate lazy parsing and quick lookup once parsing is finished
 */
class ArchiveMember {
    public:
        ArchiveMember() : name_(""), offset_(0), member_(NULL) {}
        ArchiveMember(const string name, const Offset offset,
                Symtab * img = NULL) :
            name_(name), 
            offset_(offset), 
            member_(img) 
        {}

        ~ArchiveMember() {
            if( member_ != NULL ) {
                delete member_;
                member_ = NULL;
            }
        }

        const string& getName()  { return name_; }
        Offset getOffset() { return offset_; }
        Symtab * getSymtab() { return member_; }
        void setSymtab(Symtab *img) { member_ = img; }

    private:
        const string name_;
        Offset offset_;
        Symtab *member_;
};

class Archive : public AnnotatableSparse {
   public:
      static bool openArchive(Archive *&img, string filename);
      static bool openArchive(Archive *&img, char *mem_image, size_t image_size);
      static SymtabError getLastError();
      static string printError(SymtabError err);

      ~Archive();
      bool getMember(Symtab *&img, string& member_name);
      bool getMemberByOffset(Symtab *&img, Offset memberOffset);
      bool getMemberByGlobalSymbol(Symtab *&img, string& symbol_name);
      bool getAllMembers(vector<Symtab *> &members);
      bool isMemberInArchive(string& member_name);
      bool findMemberWithDefinition(Symtab *&obj, string& name);
      std::string name();

   private:
      Archive(string &filename, bool &err);
      Archive(char *mem_image, size_t image_size, bool &err);

      /**
       * This method is architecture specific
       *
       * Post-condition:
       *        sets serr and errMsg if there is an error 
       *        sets Symtab field of passed ArchiveMember
       */
      bool parseMember(Symtab *&img, ArchiveMember *member);

      /**
       * This method is architecture specific
       *
       * Post-condition:
       *        sets serr and errMsg if there is an error
       */
      bool parseSymbolTable();      

      MappedFile *mf;

      //architecture specific data - 
      //For ELF the elf pointer for the archive
      //NONE as of now for xcoff
      void *basePtr;

      dyn_hash_map<string, ArchiveMember *> membersByName;
      dyn_hash_map<Offset, ArchiveMember *> membersByOffset;
      std::multimap<string, ArchiveMember *> membersBySymbol;

      // The symbol table is lazily parsed
      bool symbolTableParsed;

      // A vector of all Archives. Used to avoid duplicating
      // an Archive that already exists.
      static vector<Archive *> allArchives;

      static SymtabError serr;
      static std::string errMsg;
};

}//namespace SymtabAPI
}//namespace Dyninst
#endif
