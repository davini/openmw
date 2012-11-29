
#include "document.hpp"

CSMDoc::Document::Document (const std::string& name)
{
    mName = name; ///< \todo replace with ESX list

    connect (&mUndoStack, SIGNAL (cleanChanged (bool)), this, SLOT (modificationStateChanged (bool)));

     // dummy implementation -> remove when proper save is implemented.
    mSaveCount = 0;
    connect (&mSaveTimer, SIGNAL(timeout()), this, SLOT (saving()));

     // dummy implementation -> remove when proper verify is implemented.
    mVerifyCount = 0;
    connect (&mVerifyTimer, SIGNAL(timeout()), this, SLOT (verifying()));
}

QUndoStack& CSMDoc::Document::getUndoStack()
{
    return mUndoStack;
}

int CSMDoc::Document::getState() const
{
    int state = 0;

    if (!mUndoStack.isClean())
        state |= State_Modified;

    if (mSaveCount)
        state |= State_Locked | State_Saving;

    if (mVerifyCount)
        state |= State_Locked | State_Verifying;

    return state;
}

const std::string& CSMDoc::Document::getName() const
{
    return mName;
}

void CSMDoc::Document::save()
{
    mSaveCount = 1;
    mSaveTimer.start (500);
    emit stateChanged (getState(), this);
    emit progress (1, 16, State_Saving, 1, this);
}

void CSMDoc::Document::verify()
{
    mVerifyCount = 1;
    mVerifyTimer.start (500);
    emit stateChanged (getState(), this);
    emit progress (1, 20, State_Verifying, 1, this);
}

void CSMDoc::Document::abortOperation (int type)
{
    if (type==State_Saving)
    {
        mSaveTimer.stop();
        emit stateChanged (getState(), this);
    }
    else if (type==State_Verifying)
    {
        mVerifyTimer.stop();
        emit stateChanged (getState(), this);
    }
}

void CSMDoc::Document::modificationStateChanged (bool clean)
{
    emit stateChanged (getState(), this);
}

void CSMDoc::Document::saving()
{
    ++mSaveCount;

    emit progress (mSaveCount, 16, State_Saving, 1, this);

    if (mSaveCount>15)
    {
            mSaveCount = 0;
            mSaveTimer.stop();
            mUndoStack.setClean();
            emit stateChanged (getState(), this);
    }
}

void CSMDoc::Document::verifying()
{
    ++mVerifyCount;

    emit progress (mVerifyCount, 20, State_Verifying, 1, this);

    if (mVerifyCount>19)
    {
            mVerifyCount = 0;
            mVerifyTimer.stop();
            emit stateChanged (getState(), this);
    }
}

const CSMWorld::Data& CSMDoc::Document::getData() const
{
    return mData;
}

CSMWorld::Data& CSMDoc::Document::getData()
{
    return mData;
}