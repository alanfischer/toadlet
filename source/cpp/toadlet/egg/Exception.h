/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_EGG_EXCEPTION_H
#define TOADLET_EGG_EXCEPTION_H

#include <toadlet/Types.h>

namespace toadlet{
namespace egg{

class TOADLET_API Exception{
public:
	Exception(int error=0){init(error,NULL,NULL);}
	Exception(const char *description){init(0,NULL,description);}
	Exception(int error,const char *description){init(error,NULL,description);}
	Exception(int error,const char *category,const char *description){init(error,category,description);}
	Exception(const Exception &ex){init(ex.error,ex.category,ex.description);}
	virtual ~Exception() throw(){delete category;delete description;}

	virtual const char *what() const throw(){return description;}

	int getError() const throw(){return error;}
	const char *getCategory() const throw(){return category;}
	const char *getDescription() const throw(){return description;}

	void init(int error,const char *category,const char *description){
		this->error=error;
		if(category==NULL){this->category=NULL;	}
		else{
			this->category=new char[strlen(category)+1];
			strcpy(this->category,category);
		}
		if(description==NULL){this->description=NULL;}
		else{
			this->description=new char[strlen(description)+1];
			strcpy(this->description,description);
		}
	}

	int error;
	char *category;
	char *description;
};

}
}

#endif
