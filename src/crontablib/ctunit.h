/***************************************************************************
 *   CT Unit of Time Interval Header                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTUNIT_H
#define CTUNIT_H

#include <QString>
#include <QList>

/**
 * A cron table unit parser and tokenizer.
 * Parses/tokenizes unit such as "0-3,5,6,10-30/5"
 * Also provides default natural language description.
 */
class CTUnit {
	
protected:
	CTUnit(int min, int max, const QString& tokStr = "");

	/**
	 * Get default natural language description.
	 */
	virtual QString genericDescribe(const QList<QString>& label) const;
	
public:

	/**
	 * Base initial image as empty and copy enabled intervals.
	 */
	CTUnit(const CTUnit& source);

	/**
	 * Destructor.
	 */
	virtual ~CTUnit();

	/**
	 * Copy enabled intervals.
	 */
	void operator =(const CTUnit& unit);

	/**
	 * Returns tokenization
	 */
	QString exportUnit();
	
	/**
	 * Parses unit such as "0-3,5,6,10-30/5".
	 * And initialize array of enabled intervals.
	 */
	void initialize(const QString& tokStr = "");

	/**
	 * Parses unit such as "0-3,5,6,10-30/5".
	 * Does not initialize array of enabled intervals.
	 */
	void parse(const QString& tokenString = "");

	/**
	 * Tokenizes unit into string such as
	 * "0,1,2,3,5,6,10,15,20,25,30".
	 */
	QString tokenize() const;

	/**
	 * Lower bound.
	 */
	int begin();

	/**
	 * Upper bound.
	 */
	int end();

	/**
	 * Accessor.
	 */
	bool get(int pos) const;

	/**
	 * Mutator.
	 */
	void set(int pos, bool value);

	/**
	 * Enable.
	 */
	void enable(int pos);

	/**
	 * Disable.
	 */
	void disable(int pos);

	/**
	 * Indicates whether enabled intervals have been modified.
	 */
	bool dirty() const;

	/**
	 * Total count of enabled intervals.
	 */
	int count() const;

	/**
	 * Mark changes as applied.
	 */
	void apply();

	/**
	 * Mark cancel changes and revert to initial or last applied
	 * image.
	 */
	void cancel();
	
private:
	int min;
	int max;

	int fieldToValue(const QString& entry) const;
	bool isDirty;
	
	QList<bool> enabled;
	QList<bool> initialEnabled;
	
	QString initialTokStr;

public:

	/**
	 * Constant indicating short format.
	 */
	static const bool shortFormat = 0;

	/**
	 * Constant indicating long format.
	 */
	static const bool longFormat = 1;

};

#endif // CTUNIT_H
