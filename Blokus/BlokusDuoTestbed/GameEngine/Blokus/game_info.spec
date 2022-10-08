# Specification  for updating elements in Game Status Repository according to packet

# initialize game status from game setting
[from_game_setting]
	[[initialize]]
		isStrategySpecific = N
		[[[updatingList]]]
		    [[[[1]]]]
				operation = 'assign'
				gameElem = map
				updatingElem = {}
		    [[[[2]]]]
				operation = 'assign'
				gameElem = freeBlocks
				updatingElem = []
		    [[[[3]]]]
				operation = 'assign'
				gameElem = stepLog
				updatingElem = []



# update game status from packet
[from_packet]
	# update board status
	[[initialize_map]]
	    isStrategySpecific = N
	    [[[updatingList]]]
	        [[[[1]]]]
	            operation = 'assign'
	            gameElem = map
	            updatingElem = $map
	[[initialize_free_blocks]]
	    isStrategySpecific = N
	    [[[updatingList]]]
	        [[[[1]]]]
	            operation = 'assign'
	            gameElem = freeBlocks
	            updatingElem = $freeBlocks
	[[add_block]]
	    isStrategySpecific = N
	    [[[updatingList]]]
	        [[[[1]]]]
	            operation = 'assign'
	            gameElem = map
	            updatingElem = $map
	[[update_free_blocks]]
	    isStrategySpecific = N
	    [[[updatingList]]]
	        [[[[1]]]]
	            operation = 'assign'
	            gameElem = freeBlocks
	            updatingElem = $freeBlocks
	[[update_step_log]]
	    isStrategySpecific = N
	    [[[updatingList]]]
	        [[[[1]]]]
	            operation = 'assign'
	            gameElem = stepLog
	            updatingElem = $stepLog
	[[game_over]]
	    isStrategySpecific = N
	    [[[updatingList]]]
	        [[[[1]]]]
	            operation = 'assign'
	            gameElem = stepLog
	            updatingElem = $stepLog
