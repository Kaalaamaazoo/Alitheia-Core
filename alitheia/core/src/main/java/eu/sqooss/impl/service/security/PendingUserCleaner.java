/*
 * This file is part of the Alitheia system, developed by the SQO-OSS
 * consortium as part of the IST FP6 SQO-OSS project, number 033331.
 *
 * Copyright 2007 - 2010 - Organization for Free and Open Source Software,  
 *                Athens, Greece.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

package eu.sqooss.impl.service.security;

import java.util.Date;
import java.util.List;
import java.util.TimerTask;

import eu.sqooss.impl.service.security.utils.UserManagerDatabase;
import eu.sqooss.service.db.PendingUser;

class PendingUserCleaner extends TimerTask {

    private UserManagerDatabase db;
    private long expirationPeriod;
    
    PendingUserCleaner(UserManagerDatabase db, long expirationPeriod) {
        this.db = db;
        this.expirationPeriod = expirationPeriod;
    }
    
    /**
     * @see java.util.TimerTask#run()
     */
    @Override
    public void run() {
        // FIXME : Need to get access to DBService to start and end the DB session
        List<?> pendingUser = db.getFirstPendingUser();
        if ((pendingUser != null) && (pendingUser.size() != 0)) {
            PendingUser firstPendingUser = (PendingUser) pendingUser.get(0);
            Date expirationDate = new Date(firstPendingUser.getCreated().getTime() + expirationPeriod);
            if (System.currentTimeMillis() >= expirationDate.getTime()) {
                db.deletePendingUser(firstPendingUser);
            }
        }
    }
    
}

//vi: ai nosi sw=4 ts=4 expandtab
