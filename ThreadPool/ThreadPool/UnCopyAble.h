#pragma once

class UnCopyAble
{
protected:
	UnCopyAble() {}
	~UnCopyAble() {}
private:
	UnCopyAble(const UnCopyAble&) {}
	UnCopyAble& operator=(const UnCopyAble&) {}
};


