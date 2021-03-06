/***************************************************************************
                          hierarchy.h  -  class Hierarchy
                             -------------------
    begin                : jeu avr 17 2003
    copyright            : (C) 2003 by Michael CATANZARITI
    email                : mcatan@free.fr
 ***************************************************************************/

/***************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.      *
 *                                                                         *
 * This software is published under the terms of the Apache Software       *
 * License version 1.1, a copy of which has been included with this        *
 * distribution in the LICENSE.txt file.                                   *
 ***************************************************************************/

#ifndef _LOG4CXX_HIERARCHY_H
#define _LOG4CXX_HIERARCHY_H

#include <log4cxx/spi/loggerrepository.h>
#include <log4cxx/spi/loggerfactory.h>
#include <vector>
#include <map>
#include <log4cxx/provisionnode.h>
#include <log4cxx/helpers/objectimpl.h>

namespace log4cxx
{
	namespace spi
	{
		class HierarchyEventListener;
		typedef log4cxx::helpers::ObjectPtr<HierarchyEventListener>
			HierarchyEventListenerPtr;
		typedef std::vector<HierarchyEventListenerPtr> HierarchyEventListenerList;
	}

	/**
	This class is specialized in retrieving loggers by name and also
	maintaining the logger hierarchy.

	<p><em>The casual user does not have to deal with this class
	directly.</em>

	<p>The structure of the logger hierarchy is maintained by the
	#getLogger method. The hierarchy is such that children link
	to their parent but parents do not have any pointers to their
	children. Moreover, loggers can be instantiated in any order, in
	particular descendant before ancestor.

	<p>In case a descendant is created before a particular ancestor,
	then it creates a provision node for the ancestor and adds itself
	to the provision node. Other descendants of the same ancestor add
	themselves to the previously created provision node.
	*/
	class Hierarchy :
		public virtual spi::LoggerRepository,



		public virtual helpers::ObjectImpl
	{
	private:
        spi::LoggerFactoryPtr defaultFactory;
		spi::HierarchyEventListenerList listeners;
		
        typedef std::map<tstring, LoggerPtr> LoggerMap;
        LoggerMap loggers;

        typedef std::map<tstring, ProvisionNode> ProvisionNodeMap;
        ProvisionNodeMap provisionNodes;

		LoggerPtr root;
		
        int thresholdInt;
        const Level * threshold;
		
        bool emittedNoAppenderWarning;
        bool emittedNoResourceBundleWarning;

        /**
        Map synchronization
        */
        helpers::CriticalSection mapCs;
		
    public:
		/**
		Create a new logger hierarchy.
		@param root The root of the new hierarchy.
	 
        */
        Hierarchy(LoggerPtr root);

        ~Hierarchy();
		
	public:
		void addHierarchyEventListener(spi::HierarchyEventListenerPtr listener);
		
		/**
		This call will clear all logger definitions from the internal
		hashtable. Invoking this method will irrevocably mess up the
		logger hierarchy.

		<p>You should <em>really</em> know what you are doing before
		invoking this method.
		*/
	public:
		void clear();
		
	public:
		void emitNoAppenderWarning(LoggerPtr logger);
		
		/**
		Check if the named logger exists in the hierarchy. If so return
		its reference, otherwise returns <code>null</code>.
		
		  @param name The name of the logger to search for.
		  
		*/
	public:
		LoggerPtr exists(const tstring& name);
		
		/**
		The string form of {@link #setThreshold(const Level&) setThreshold}.
		*/
	public:
		void setThreshold(const tstring& levelStr);
		
        /**
        Enable logging for logging requests with level <code>l</code> or
        higher. By default all levels are enabled.
		
		@param l The minimum level for which logging requests are sent to
        their appenders.  */
   	public:
		void setThreshold(const Level& l);
		
	public:
		void fireAddAppenderEvent(LoggerPtr logger, AppenderPtr appender);
		
	public:
		void fireRemoveAppenderEvent(LoggerPtr logger, AppenderPtr appender);
		
		/**
		Returns a Level representation of the <code>enable</code>
		state.
		*/
	public:
		const Level& getThreshold();
		
		/**
		Return a new logger instance named as the first parameter using
		the default factory.
		
		<p>If a logger of that name already exists, then it will be
		returned.  Otherwise, a new logger will be instantiated and
		then linked with its existing ancestors as well as children.

		@param name The name of the logger to retrieve.
			
		*/
	public:
		LoggerPtr getLogger(const tstring& name);
		
		/**
		Return a new logger instance named as the first parameter using
		<code>factory</code>.

		<p>If a logger of that name already exists, then it will be
		returned.  Otherwise, a new logger will be instantiated by the
		<code>factory</code> parameter and linked with its existing
		ancestors as well as children.

		@param name The name of the logger to retrieve.
		@param factory The factory that will make the new logger instance.
			
		*/
	public:
		LoggerPtr getLogger(const tstring& name, spi::LoggerFactoryPtr factory);
		
		/**
		Returns all the currently defined loggers in this hierarchy as
		a LoggerList.

		<p>The root logger is <em>not</em> included in the returned
		LoggerList.  */
	public:
		LoggerList getCurrentLoggers();
		
		/**
		Get the root of this hierarchy.
		*/
	public:
		LoggerPtr getRootLogger();
		
		/**
		This method will return <code>true</code> if this repository is
		disabled for <code>level</code> object passed as parameter and
		<code>false</code> otherwise. See also the 
		{@link #setThreshold(const Level&) setThreshold} method.  */
	public:
		bool isDisabled(int level);
		
		/**
		Reset all values contained in this hierarchy instance to their
		default.  This removes all appenders from all categories, sets
		the level of all non-root categories to <code>null</code>,
		sets their additivity flag to <code>true</code> and sets the level
		of the root logger to {@link Level#DEBUG DEBUG}.  Moreover,
		message disabling is set its default "off" value.
		
		<p>Existing categories are not removed. They are just reset.
	  
		<p>This method should be used sparingly and with care as it will
		block all logging until it is completed.</p>
		*/
	public:
		void resetConfiguration();
		
		/**
		Used by subclasses to add a renderer to the hierarchy passed as parameter.
		*/
	public:
		/**
		Shutting down a hierarchy will <em>safely</em> close and remove
		all appenders in all categories including the root logger.
		
		<p>Some appenders such as {@link net::SocketAppender SocketAppender}
		and AsyncAppender need to be closed before the
		application exists. Otherwise, pending logging events might be
		lost.

		<p>The <code>shutdown</code> method is careful to close nested
		appenders before closing regular appenders. This is allows
		configurations where a regular appender is attached to a logger
		and again to a nested appender.
		*/
	public:

		void shutdown();
		
		
		/**
		This method loops through all the *potential* parents of
		'cat'. There 3 possible cases:

		1) No entry for the potential parent of 'cat' exists

		We create a ProvisionNode for this potential parent and insert
		'cat' in that provision node.

		2) There entry is of type Logger for the potential parent.

		The entry is 'cat's nearest existing parent. We update cat's
		parent field with this entry. We also break from the loop
		because updating our parent's parent is our parent's
		responsibility.

		3) There entry is of type ProvisionNode for this potential parent.

		We add 'cat' to the list of children for this potential parent.
		*/
	private:
		void updateParents(LoggerPtr logger);
		
		/**
		We update the links for all the children that placed themselves
		in the provision node 'pn'. The second argument 'cat' is a
		reference for the newly created Logger, parent of all the
		children in 'pn'

		We loop on all the children 'c' in 'pn':

		If the child 'c' has been already linked to a child of
		'cat' then there is no need to update 'c'.

		Otherwise, we set cat's parent field to c's parent and set
		c's parent field to cat.
		*/
	private:

		void updateChildren(ProvisionNode& pn, LoggerPtr logger);
	};
}; //namespace log4cxx

#endif //_LOG4CXX_HIERARCHY_H
