
#include "precomp.h"
 
LIST_ENTRY WhiteNameListHeader;
KSPIN_LOCK WhiteNameListLock;


VOID InitWhiteProcNameListAndLock()
{
	DbgPrint("Enter InitWhiteProcNameListAndLock()\n");

	InitializeListHead(&WhiteNameListHeader);
	KeInitializeSpinLock(&WhiteNameListLock);

	DbgPrint("Leave InitWhiteProcNameListAndLock()\n");
}


VOID AddNameToWhiteNameList(PWSTR xxName, PWSTR xxFileType)
{
	PWHITE_NAME_LIST whiteListNode;
	BOOL bRet;
	KIRQL oldIrql;

	bRet = IsNameInWhiteNameList(xxName, xxFileType);
	if (bRet)
	{
		return ;
	}

    // 分配指定类型的池存储器和指针返回到所分配的块
	whiteListNode = (PWHITE_NAME_LIST)ExAllocatePoolWithTag(NonPagedPool,sizeof(WHITE_NAME_LIST),'dsf ');
	if (whiteListNode == NULL)
	{
		return ;
	}

	wcscpy(whiteListNode->xxName,xxName);
	wcscpy(whiteListNode->xxFileType, xxFileType);
	KeAcquireSpinLock(&WhiteNameListLock,&oldIrql);
	InsertTailList(&WhiteNameListHeader,&whiteListNode->listNode);// 插入在双重链接列表的尾部的条目LIST_ENTRY结构
	KeReleaseSpinLock(&WhiteNameListLock,oldIrql);
}


VOID DelNameFromWhiteNameList(PWSTR xxName, PWSTR xxFileType)
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	KIRQL oldIrql;

	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode!=&WhiteNameListHeader; pListNode = pListNode->Flink)
		{
			whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
			if ((wcscmp(whiteList->xxName,xxName) == 0) && (wcscmp(whiteList->xxFileType, xxFileType) == 0))
			{
				KeAcquireSpinLock(&WhiteNameListLock,&oldIrql);
				RemoveEntryList(&whiteList->listNode);
				ExFreePoolWithTag(whiteList,'dsf ');
				KeReleaseSpinLock(&WhiteNameListLock,oldIrql);
				return ;

			}
		}
	}
}


BOOL IsNameInWhiteNameList(PWSTR xxName, PWSTR xxFileType)
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;

	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode != &WhiteNameListHeader; pListNode = pListNode->Flink)
		{
			whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
			if (wcscmp(whiteList->xxName,xxName)==0 && wcscmp(whiteList->xxFileType,xxFileType) == 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL SearchIsProtect(PWSTR DirPath,PWSTR FileName)
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	DbgPrint("***Enter SearchIsProtect()***\n");
	if (!IsListEmpty(&WhiteNameListHeader))
	{
		for (pListNode = WhiteNameListHeader.Flink; pListNode != &WhiteNameListHeader; pListNode = pListNode->Flink)
		{
			//CONTAINING_RECORD这个宏的作用是：根据一个结构体实例中的某一个成员的地址，取到整个结构体实例的地址
			whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
			if (RtlPatternMatch(whiteList->xxName,DirPath))
			{
				if (RtlPatternMatch(whiteList->xxFileType, FileName))
				{
					DbgPrint("***Leave SearchIsProtect()***\n");
					return TRUE;
				}

			}
		}
	}

	DbgPrint("***Leave SearchIsProtect()***\n");
	return FALSE;
}


VOID ClearWhiteNameList()
{
	PWHITE_NAME_LIST whiteList;
	PLIST_ENTRY pListNode;
	KIRQL oldIrql;

	KeAcquireSpinLock(&WhiteNameListLock,&oldIrql);
	while(!IsListEmpty(&WhiteNameListHeader))
	{
		pListNode = RemoveTailList(&WhiteNameListHeader);
		whiteList = CONTAINING_RECORD(pListNode,WHITE_NAME_LIST,listNode);
		if (whiteList)
		{
			ExFreePoolWithTag(whiteList,'dsf ');
		}
	}
	KeReleaseSpinLock(&WhiteNameListLock,oldIrql);
}


static int __inline Lower(int c)
{
	if ((c >= L'A') && (c <= L'Z'))
	{
		return(c + (L'a' - L'A'));
	}
	else
	{
		return(c);
	}
}


BOOL RtlPatternMatch(WCHAR * pat, WCHAR * str)
{
	register WCHAR * s;
	register WCHAR * p;
	BOOL star = FALSE;

loopStart:
	for (s = str, p = pat; *s; ++s, ++p) {
		switch (*p) {
		 case L'?':
			 if (*s == L'.') goto starCheck;
			 break;
		 case L'*':
			 star = TRUE;
			 str = s, pat = p;
			 if (!*++pat) return TRUE;
			 goto loopStart;
		 default:
			 if (Lower(*s) != Lower(*p))
				 goto starCheck;
			 break;
		} 
	} 
	if (*p == L'*') ++p;
	return (!*p);

starCheck:
	if (!star) return FALSE;
	str++;
	goto loopStart;
}