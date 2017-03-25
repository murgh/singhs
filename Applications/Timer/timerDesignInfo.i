%module timerDesignInfo
%{
        
        #include "timerPin.hxx"
        #include "timerEdge.hxx"
        #include "timerUtils.hxx"
        #include "timerLibData.hxx"
                
        extern std::list<timerLibData *> theLibraryInfo; 
        void create_node (char * circuit_name,
		          char * name, 
		          bool isClock, 
		          bool isData,
		          timerPinIdentifier pinId,
	       	          timerPinDirection pinDir); 

        void create_edge (char * circuit_name,
		          char * source,
		          char * sink); 

        extern timerLibData *  add_or_get_library (char * libName);

        extern timerLibCell * add_or_get_cell (timerLibData * lib, char * cellName); 

        extern timerLibPin * add_or_get_pin (timerLibCell * cell, char * pinName); 

        void add_pin_direction (timerLibPin * pin, char * dir);

        void add_pin_cap (timerLibPin * pin, float cap);

        void add_pin_relPin (timerLibPin * pin, char * relPin);

        timerLibArc * add_timing_arc (timerLibCell * cell, char * source, char * sink);

        void add_timing_type (timerLibArc * arc, char * timing_type);

        void add_timing_sense (timerLibArc * arc, char * timing_sense); 

        extern char * getLibName (timerLibData *);
%}

        void create_node (char * circuit_name,
		          char * name, 
		          bool isClock, 
		          bool isData,
		          timerPinIdentifier pinId,
	       	          timerPinDirection pinDir); 

        void create_edge (char * circuit_name,
		          char * source,
		          char * sink); 

        timerLibData *  add_or_get_library (char * libName);

        timerLibCell * add_or_get_cell (timerLibData * lib, char * cellName); 

        timerLibPin * add_or_get_pin (timerLibCell * cell, char * pinName); 

        void add_pin_direction (timerLibPin * pin, char * dir);

        void add_pin_cap (timerLibPin * pin, float cap);

        void add_pin_relPin (timerLibPin * pin, char * relPin);

        char * getLibName (timerLibData *);

        void add_timing_sense (timerLibArc * arc, char * timing_sense);

        timerLibArc * add_timing_arc (timerLibCell * cell, char * source, char * sink);

        void add_timing_type (timerLibArc * arc, char * timing_type);
