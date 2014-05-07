#include <stdlib.h>
template<class Object>
SList<Object>::SList( ) {
  init( );
}

template<class Object>
SList<Object>::SList( const SList &rhs ) {
  init( );
  *this = rhs;                                   // then assign rhs to this.
}

template<class Object> 
void SList<Object>::init( ) {
  for ( int i = 0; i < LEVEL; i++ ) {  // for each level
    // create the left most dummy nodes;
    header[i] = new SListNode<Object>;
    header[i]->prev = NULL;
    header[i]->down = ( i > 0 ) ? header[i - 1] : NULL;
    header[i]->up = NULL;
    if ( i > 0 ) header[i - 1]->up = header[i];

    // create the right most dummy nodes;
    header[i]->next = new SListNode<Object>;
    header[i]->next->next = NULL;
    header[i]->next->prev = header[i];
    header[i]->next->down = ( i > 0 ) ? header[i - 1]->next : NULL;
    header[i]->next->up = NULL;
    if ( i > 0 ) header[i - 1]->next->up = header[i]->next;
  }

  // reset cost.
  cost = 0;
}

template<class Object>
SList<Object>::~SList( ) {
  //clear( );                                      // delete items starting 1st
  for ( int i = 0; i < LEVEL; i++ ) {
    delete header[i]->next;                      // delete the right most dummy
    delete header[i];                            // delete the left most dummy
  }
}

template<class Object>
bool SList<Object>::isEmpty( ) const {
  return ( header[0]->next->next == NULL );
}

template<class Object>
int SList<Object>::size( ) const {
  SListNode<Object> *p = header[0]->next; // at least the right most dummy
  int size = 0;

  for ( ; p->next != NULL; p = p->next, ++size );   
  return size;
}

template<class Object>
void SList<Object>::clear( ) {
  for ( int i = 0; i < LEVEL; i++ ) {        // for each level
    SListNode<Object> *p = header[i]->next;  // get the 1st item 
    while ( p->next != NULL ) {              // if this is not the left most
      SListNode<Object> *del = p;          
      p = p->next;                           // get the next item
      delete del;                            // delete the current item
    }

    header[i]->next = p;                     // p now points to the left most 
  }                                          // let the right most point to it
}

template<class Object>
void SList<Object>::insert( const Object &obj ) {
  // right points to the level-0 item before which a new object is inserted.
  SListNode<Object> *right = searchPointer( obj );
  SListNode<Object> *upptr = NULL;
  if ( right->next != NULL && right->item == obj )  
    // there is an identical object
    return;
	
   SListNode<Object> *objptr = new SListNode<Object>();  // allocate space
   objptr->item = Object(obj);
   objptr->down = NULL;
   objptr->up = NULL;
   objptr->next = NULL;
   objptr->prev = NULL;
   
  //insert into first level of linked list (horizontally)
  objptr->prev = right->prev; //obj prev points to prev of right
  objptr->next = right;       //obj next points to right
  right->prev->next = objptr; //item to the left of obj points to obj
  right->prev = objptr;       //right prev is now object
  
  //insert into upper levels (vertically)
  for(int i = 1;i<=5;i++){       //maximum 5 levels start at level 1
	int coinflip;
    coinflip = rand() % 2;      //find the "height" of the inserted object in list
	if(coinflip!=1){           //stop if the randomnumb is not 1
	  return;
	}
	
	 upptr = objptr;
	SListNode<Object> *topobj = new SListNode<Object>(); //create highest object
	topobj->down = NULL;
   topobj->up = NULL;
   topobj->next = NULL;
   topobj->prev = NULL;
	
	for(int j=0;j<i;j++){               //creates a node that is a copy of object at the highest point
		if(upptr->up==NULL){   	        //highest obj found
			topobj->item = Object(obj);         //contains obj item
			topobj->down = upptr;          //topobj down points to pointer var "up"
			upptr->up = topobj;           //up points to topobj
			break;
		}
		upptr = upptr->up;                        //places top object in correct location
	} //end j for loop
	SListNode<Object> *current = objptr;          // current keeps track of my place in the linked list
    while(current->prev!=NULL){                  //checks all previous nodes to left of topobj
		current = current->prev;
		for(int top=0;top<i;top++){              //attempt to reach ith level at current node
			if(current==NULL||current->up==NULL){// current node does not have i levels exit 
				break;
			}
			if(top==(i-1)){                       //current node has i levels
				current->up->next = topobj;
				topobj->prev = current->up;
			}
			current = current->up; //move up
		} //end for loop
	} //end while loop

	current = objptr;
	while(current!=NULL&&current->next!=NULL){ 	//this while loop checks the list to the right of topobj
		current = current->next;
		for(int top=0;top<i;top++){                //attempt to reach ith level at current node
			if(current==NULL||current->up==NULL){  // current node does not have i levels exit 
				break;
			}
			if(top==(i-1)){            //current node has i levels
				current->up->prev = topobj;
				topobj->next = current->up;
			}
			current = current->up;   //move up
		} //end for loop
	} //end while
  } //end i for loop
 } //end insert func


template<class Object> 
bool SList<Object>::find( const Object &obj ) {
  // p oints to the level-0 item close to a given object
  SListNode<Object> *p = searchPointer( obj );

  return ( p->next != NULL && p->item == obj );     // true if obj was found
}

template<class Object>
SListNode<Object> *SList<Object>::searchPointer( const Object &obj ) {
  SListNode<Object> *p = header[LEVEL - 1];     // start from the top left
  while ( p->down != NULL ) {                   // toward level 0
    p = p->down;                                // shift down once
    cost++;

    if ( p->prev == NULL ) {                    // at the left most item
      if ( p->next->next == NULL )              // no intermediate items
        continue;
      else {                                    // some intermadiate items
        if ( p->next->item <= obj )             // if 1st item <= obj
          p = p->next;                          // shift right to item 1
        cost++;
      }
    }

    while ( p->next->next != NULL && p->next->item <= obj ) {
      // shift right through intermediate items as far as the item value <= obj
      p = p->next;
      cost++;
    }
  }

  // now reached the bottom. shift right once if the current item < obj
  if ( p->prev == NULL || p->item < obj ) {
    p = p->next;
    cost++;
  }
  return p; // return the pointer to an item >= a given object.
}

template<class Object>
void SList<Object>::remove( const Object &obj ) {
  // p points to the level-0 item to delete
  SListNode<Object> *p = searchPointer( obj );      
 
  // validate if p is not the left most or right most and exactly contains the
  // item to delete
  if ( p->prev == NULL || p->next == NULL || p->item != obj )
    return;

  // Implement the rest by yourself /////////////////////////////////////////
  //create loop to go up each level of p 
  //at highest level of p make the prev node point to next node and delete the current(highest) node
  //process repeates until level 0 of p is deleted
  
  while(p->up!=NULL){   //moves p vertically to the top node
	  p = p->up;
  }
  SListNode<Object> *bellowp = p->down; //extra pointer keeps track of position when p is deleted
  while(bellowp!=NULL){ //deletes all but the last node on level 0
	 SListNode<Object> *previousnode = p->prev;
	  SListNode<Object> *nextnode = p->next;
	  if(previousnode!=NULL&&nextnode!=NULL){ //check if there is a previous and next node at this level
		previousnode->next = nextnode;
		nextnode->prev = previousnode;
	  }
	  else{  //set objects that pointed to p to point to null if there is no prev/next on that level
		  if(previousnode==NULL){
			  nextnode->prev = NULL; //might be header
		  }
		  if(nextnode==NULL){
			  previousnode->next = NULL;
		  }
	  }
	  if(p!=NULL){
	  delete p;
	  }
	  bellowp->up = NULL;
	  p = bellowp;
	  bellowp = p->down;
  }  //end while loop
  //delete the last (level 0) node
  p->next->prev = p->prev;
  p->prev->next = p->next;
  if(p!=NULL){
  delete p;
  p = NULL;
  }
} //end remove func
template<class Object>
const SList<Object> &SList<Object>::operator=( const SList &rhs ) {
  if ( this != &rhs ) { // avoid self-assignment
    clear( );           // deallocate old items
    
    int index;
    SListNode<Object> *rnode;
    for ( index = 0, rnode = rhs.header[0]->next; rnode->next != NULL;
	  rnode = rnode->next, ++index )
      insert( rnode->item );

    cost = rhs.cost;
  }
  return *this;
}

template<class Object>
int SList<Object>::getCost( ) const {
  return cost;
}

template<class Object>
void SList<Object>::show( ) const {
  cout << "contents:" << endl;
  for ( SListNode<Object> *col = header[0]; col != NULL; col = col->next ) {
    SListNode<Object> *row = col;
    for ( int level = 0; row != NULL && level < LEVEL; level++ ) {
      if ( row->prev == NULL )
	cout << "-inf\t";
      else if ( row->next == NULL )
	cout << "+inf\t";
      else
	cout << row->item << "\t";
      row = row->up;
    }
    cout << endl;
  }
}
