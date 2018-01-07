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

        void add_timing_arc (diganaGraph * circuit, int source, int sink, timerLibArc * arc);

        void add_timing_arc (diganaGraph * circuit, int source, int sink);

        extern timerLibData *  add_or_get_library (char * libName);

        extern timerLibCell * add_or_get_cell (timerLibData * lib, char * cellName); 

        extern timerLibPin * add_or_get_pin (timerLibCell * cell, char * pinName); 

        void add_pin_direction (timerLibPin * pin, char * dir);

        void mark_clock (timerLibPin * pin);

        void add_pin_cap (timerLibPin * pin, float cap);

        void add_pin_relPin (timerLibPin * pin, char * relPin);

        timerLibArc * add_timing_arc (timerLibCell * cell, char * source, char * sink);

        timerLibArc * get_timing_arc (timerLibCell * cell, char * source, char * sink);

        void add_timing_type (timerLibArc * arc, char * timing_type);

        void add_timing_sense (timerLibArc * arc, char * timing_sense); 

        diganaGraph * create_circuit (char * circuit, int size);

        diganaGraph * get_circuit (char * circuit);

        int add_pin (diganaGraph * circuit, char * name, int node_count);

        void add_pin_direction_io (diganaGraph * circuit, int id, char * dir, int isIO, char * p);

        void perform_timing_analysis (char * circuit);

        extern char * getLibName (timerLibData *);

        extern char * get_pin_type (timerLibCell * cell, char * pin);

        void add_clock (diganaGraph * circuit, char * name, int period, int nodeId, int isVirtual); 

        void add_IO_delay (diganaGraph * circuit, float value, int source, int input); 

        void addReportObject (int from, int through, int to);
%}

        void create_node (char * circuit_name,
		          char * name, 
		          bool isClock, 
		          bool isData,
		          timerPinIdentifier pinId,
	       	          timerPinDirection pinDir); 

        void add_timing_arc (diganaGraph * circuit, int source, int sink, timerLibArc * arc);

        void add_timing_arc (diganaGraph * circuit, int source, int sink);

        timerLibData *  add_or_get_library (char * libName);

        timerLibCell * add_or_get_cell (timerLibData * lib, char * cellName); 

        timerLibPin * add_or_get_pin (timerLibCell * cell, char * pinName); 

        int add_pin (diganaGraph * circuit, char * name, int node_count);

        void mark_clock (timerLibPin * pin);

        void add_pin_cap (timerLibPin * pin, float cap);

        void add_pin_relPin (timerLibPin * pin, char * relPin);

        char * getLibName (timerLibData *);

        void add_timing_sense (timerLibArc * arc, char * timing_sense);

        timerLibArc * add_timing_arc (timerLibCell * cell, char * source, char * sink);

        void add_timing_type (timerLibArc * arc, char * timing_type);

        diganaGraph * create_circuit (char * circuit, int size);

        diganaGraph * get_circuit (char * circuit);

        void add_pin_direction_io (diganaGraph * circuit, int id, char * dir, int isIO, char * p);

        void add_pin_direction (timerLibPin * pin, char * dir);

        void perform_timing_analysis (char * circuit);

        timerLibArc * get_timing_arc (timerLibCell * cell, char * source, char * sink);

        char * get_pin_type (timerLibCell * cell, char * pin);

        void add_clock (diganaGraph * circuit, char * name, int period, int nodeId, int isVirtual);

        void add_IO_delay (diganaGraph * circuit, float value, int nodeId, int input);

        void addReportObject (int from, int through, int to);
