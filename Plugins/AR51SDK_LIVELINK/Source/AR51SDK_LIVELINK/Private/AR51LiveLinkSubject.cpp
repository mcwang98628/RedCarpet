#include "AR51LiveLinkSubject.h"

void UAR51LiveLinkSubject::SetSubject(FName SubjectName)
{
	Subjects.Empty();
	Subjects.Add(SubjectName);
}
void UAR51LiveLinkSubject::ClearSubjects()
{
	Subjects.Empty();
}
