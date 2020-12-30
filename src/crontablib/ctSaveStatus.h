/*
    CT Hour Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CT_SAVE_STATUS
#define CT_SAVE_STATUS

#include <QString>

class CTSaveStatus {
public:
	
	CTSaveStatus() {
		this->errorStatus = false;
	}
	
	CTSaveStatus(const QString& errorMessage, const QString& detailErrorMessage) {
		this->errorStatus = true;
		this->error = errorMessage;
		this->detailError = detailErrorMessage;
	}
	
	QString errorMessage() const {
		return error;
	}

	QString detailErrorMessage() const {
		return detailError;
	}
	
	bool isError() const {
		return errorStatus;
	}
	
private:
	bool errorStatus;
	
	QString error;
	
	QString detailError;

};


#endif // CT_SAVE_STATUS
