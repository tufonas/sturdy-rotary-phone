#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../header-files/functions.h"
#define bucketPosNum 50
#define N 12

void createRelations(int32_t A[],uint32_t size_A,int32_t B[],uint32_t size_B,oneColumnRelation **S,oneColumnRelation **R){
	int32_t i;

	*S=malloc(sizeof(oneColumnRelation));
	(*S)->num_of_tuples=size_A;
	(*S)->tuples=malloc(size_A*sizeof(tuple));
	for(i=0;i<size_A;i++){
		(*S)->tuples[i].id=i+1;
		(*S)->tuples[i].value=A[i];
	}

	*R=malloc(sizeof(oneColumnRelation));
	(*R)->num_of_tuples=size_B;
	(*R)->tuples=malloc(size_B*sizeof(tuple));
	for(i=0;i<size_B;i++){
		(*R)->tuples[i].id=i+1;
		(*R)->tuples[i].value=B[i];
	}	
}

hist* createHistArray(oneColumnRelation **rel){
	int32_t i,j,*freq;
	int32_t count;
	hist *Hist;
	Hist=malloc(sizeof(Hist));
	Hist->histSize=pow(2,N);
	Hist->histArray=malloc(Hist->histSize*sizeof(histNode));	
	for(i=0;i<Hist->histSize;i++){
		Hist->histArray[i].count=0;
		Hist->histArray[i].point=0;
	}

	freq=malloc((*rel)->num_of_tuples*sizeof(int32_t));
	for(i=0;i<(*rel)->num_of_tuples;i++)
		freq[i]=-1;

	for(i=0;i<(*rel)->num_of_tuples;i++){
		count=1;
		for(j=i+1;j<(*rel)->num_of_tuples;j++){
			if((*rel)->tuples[i].value==(*rel)->tuples[j].value){
				count++;				
				freq[j]=0;//metraw mono 1 fora ton ari8mo twn emfanisewn tou value
			}
		}
		if(freq[i]!=0){//an den exw ksanaupologisei to sugkekrimeno value
			freq[i]=count;
			Hist->histArray[(*rel)->tuples[i].value%Hist->histSize].count+=freq[i];
		}
	}
	free(freq);
	return Hist;
}

hist* createSumHistArray(hist *array){
	int32_t i,nextBucket=0;
	hist *Hist;
	Hist=malloc(sizeof(Hist));
	Hist->histSize=pow(2,N);
	Hist->histArray=malloc(Hist->histSize*sizeof(histNode));
	
	for(i=0;i<Hist->histSize;i++){
		Hist->histArray[i].count=0;
		Hist->histArray[i].point=0;
	}
	for(i=0;i<array->histSize;i++){
		if(i==0){//1o bucket - vriskoume ousiastika pou 8a arxizei to epomeno bucket
			nextBucket=array->histArray[i].count;
			Hist->histArray[i].count=0;
			Hist->histArray[i].point=0;
		}
		else{
			Hist->histArray[i].count=nextBucket;//pou arxizei to trexon bucket
			Hist->histArray[i].point=nextBucket;
			nextBucket+=array->histArray[i].count;//pou 8a arxizei to epomeno bucket
		}
	}
	free(array->histArray);
	free(array);
	return Hist;
}

oneColumnRelation* createReOrderedArray(oneColumnRelation *array,hist *sumArray){
	oneColumnRelation *reOrderedArray;
	int32_t i;

	reOrderedArray=malloc(sizeof(oneColumnRelation));
	reOrderedArray->num_of_tuples=array->num_of_tuples;
	reOrderedArray->tuples=malloc(array->num_of_tuples*sizeof(tuple));

	for(i=0;i<array->num_of_tuples;i++){//to point mou dixnei pou prepei na balw to stoixeio
		reOrderedArray->tuples[sumArray->histArray[array->tuples[i].value%sumArray->histSize].point].id=array->tuples[i].id;//koitazoume ton sumArray gia na brw to offset pou 8a balw to epomeno tuple
		reOrderedArray->tuples[sumArray->histArray[array->tuples[i].value%sumArray->histSize].point++].value=array->tuples[i].value;//thn deuterh fora pou bazoume to value kanw ++ gia na deixnei sthn epomenh kenh 8esh
	}
	return reOrderedArray;
}

unsigned int hash(int32_t x,int mod) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x%mod;
}
indexHT* initiliazeIndexHT(oneColumnRelation* reOrderedArray,int32_t chainNumSize)
{
	int32_t i;
	indexHT* indexht = malloc(sizeof(indexHT));
	indexht->bucketArray = malloc(bucketPosNum*sizeof(bucketNode));
	indexht->chainNode = malloc(chainNumSize*sizeof(chainNode));
	indexht->bucketSize = bucketPosNum;
	indexht->chainSize = chainNumSize;
	for(i=0;i<bucketPosNum;i++){
		indexht->bucketArray[i].lastChainPosition = -1;
	}
	for(i=0;i<indexht->chainSize;i++){
		indexht->chainNode[i].prevchainPosition = -1;
		indexht->chainNode[i].bucketPos = -1;
	}
	return indexht;
}
indexHT* createHashTable(oneColumnRelation* reOrderedArray,int32_t start,int32_t end){

	int32_t i;
	indexHT* indexht;
	indexht = initiliazeIndexHT(reOrderedArray,end-start+1);

	/*for(i=0;i<reOrderedArray->num_of_tuples;i++)
	{
		printf("reOrdered[%d]->id=%d  ->value=%d\n",i,reOrderedArray->tuples[i].id,reOrderedArray->tuples[i].value);
	}*/

	for(i=start;i<=end;i++){
		if(indexht->bucketArray[hash(reOrderedArray->tuples[i].value,bucketPosNum)].lastChainPosition == -1)//elegxos an to h 8esh tou bucket einai kenh
		{
			indexht->bucketArray[hash(reOrderedArray->tuples[i].value,bucketPosNum)].lastChainPosition = i-start;		//i-start gia na paroume thn swsth thesh apo to reorder
			indexht->chainNode[i-start].bucketPos=i;
		}
		else
		{
			//printf("mpika\n");
			indexht->chainNode[i-start].prevchainPosition = indexht->bucketArray[hash(reOrderedArray->tuples[i].value,bucketPosNum)].lastChainPosition;
			indexht->chainNode[i-start].bucketPos = i;
			indexht->bucketArray[hash(reOrderedArray->tuples[i].value,bucketPosNum)].lastChainPosition = i-start;
		}
	}
	return indexht;
}
void deleteHashTable(indexHT **ht)
{
	free((*ht)->bucketArray);				/*diagrafh tou pinaka bucket*/
	free((*ht)->chainNode);					/*diagrafh tou pinaka chain*/
	free((*ht));							/*diagrafh olhs ths domhs*/
}
void compareRelations(indexHT *ht,oneColumnRelation *array,int32_t start,int32_t end,oneColumnRelation *hashedArray,resultList *resList,int32_t fromArray){
	int32_t i,chain_offset,bucket_offset;

	for(i=start;i<=end;i++){
		if(ht->bucketArray[hash(array->tuples[i].value,bucketPosNum)].lastChainPosition == -1){//an einai -1 tote den exoume match opote to agnwoume
			continue;
		}
		else{
			chain_offset=ht->bucketArray[hash(array->tuples[i].value,bucketPosNum)].lastChainPosition;//pairnw ws offset thn teleutaia 8esh tou chain
			bucket_offset=ht->chainNode[chain_offset].bucketPos;
			while(1){
				if(array->tuples[i].value==hashedArray->tuples[bucket_offset].value){//elegxoume an exoun idio value
					insertResult(resList,hashedArray->tuples[bucket_offset].id,array->tuples[i].id,fromArray);
				}
				chain_offset=ht->chainNode[chain_offset].prevchainPosition;//to neo offset einai apo to prevchainPosition
				if(chain_offset==-1)break;//den exei alla stoixeia na doume
				bucket_offset = ht->chainNode[chain_offset].bucketPos;
			}
		}
	}
}

resultList *initializeResultList(void){
	resultList *list=malloc(sizeof(resultList));
	list->start=NULL;
	list->end=NULL;
	list->numberOfNodes=0;
	return list;
}
void insertResult(resultList *list,uint32_t id1,uint32_t id2,int32_t fromArray){
	int32_t numberoftuples=(1024*1024)/sizeof(rowResult);
	//printf("numberOfResults=%d  %ld\n",numberoftuples,sizeof(rowResult));
	if(list->end==NULL){//kenh lista
		list->start=malloc(sizeof(resultNode));
		list->end=list->start;
		list->numberOfNodes=1;
		list->numberOfResults=1;

		list->start->row_Array=malloc(numberoftuples*sizeof(rowResult));
		list->start->next=NULL;
		list->start->rowSize=1;

		if(fromArray == 0){
			list->start->row_Array[0].idR=id1;
			list->start->row_Array[0].idS=id2;
		}else if(fromArray==1){
			list->start->row_Array[0].idR=id2;
			list->start->row_Array[0].idS=id1;
		}else{
			list->start->row_Array[0].idR=id1;
		}
	}
	else{
		list->numberOfResults++;
		if( numberoftuples > list->end->rowSize ){//exei xwro

			if(fromArray == 0){
				list->end->row_Array[list->end->rowSize].idR=id1;
				list->end->row_Array[list->end->rowSize].idS=id2;
			}else if(fromArray==1){
				list->end->row_Array[list->end->rowSize].idR=id2;
				list->end->row_Array[list->end->rowSize].idS=id1;
			}else{
				list->start->row_Array[0].idR=id1;
			}
			list->end->rowSize++;
		}
		else{//ftiaxnw neo kombo
			list->end->next=malloc(sizeof(resultNode));
			list->end=list->end->next;
			list->numberOfNodes++;

			list->end->row_Array=malloc(numberoftuples*sizeof(rowResult));
			list->end->next=NULL;
			list->end->rowSize=1;

			if(fromArray == 0){
				list->end->row_Array[0].idR=id1;
				list->end->row_Array[0].idS=id2;
			}else if(fromArray==1){
				list->end->row_Array[0].idR=id2;
				list->end->row_Array[0].idS=id1;
			}else{
				list->start->row_Array[0].idR=id1;
			}
		}
	}
}
void printResults(resultList *list){
	resultNode *temp;
	temp =list->start;
	printf("\nRowidR\t\tRowidS\n");
	while(temp!=NULL){
		for(int i=0;i<temp->rowSize;i++){
			printf("%d\t\t%d\n",temp->row_Array[i].idR,temp->row_Array[i].idS);
		}
		temp=temp->next;
	}
	printf("\n%d Results\n\n",list->numberOfResults);
}
void createHT_CompareBuckets(resultList* resList,hist *histSumArrayR,hist *histSumArrayS,oneColumnRelation *RR, oneColumnRelation *RS,int32_t size_A,int32_t size_B)
{
	indexHT* ht;
	int32_t startR=0,endR,startS=0,endS,counter = 0,cnt=0;
	while(1)
	{
		if(counter+1 > histSumArrayR->histSize)break;		//counter+1 giati einai h thesh gia to end ,dld an h thesh pou tha einai to end einai ektos oriwn break//
		startR = histSumArrayR->histArray[counter].count;
		startS = histSumArrayS->histArray[counter].count;
		if(counter+1==histSumArrayR->histSize){
			endR=size_A-1;
			endS=size_B-1;
		}else{
		endR = histSumArrayR->histArray[counter+1].count-1;
		endS = histSumArrayS->histArray[counter+1].count-1;
		}

		counter++;
		if(endR+1 == startR || endS+1 == startS) continue;  //elegxos an kapoio einai keno , opote den bgazei apotelesma
		cnt++;
		if((endR - startR) >= (endS - startS))//kanoume hash to pio mikro bucket
		{
			ht=createHashTable(RS,startS,endS);
			compareRelations(ht,RR,startR,endR,RS,resList,0);		// 0 -> hashedRs
			deleteHashTable(&ht);
		}
		else
		{
			ht=createHashTable(RR,startR,endR);
			compareRelations(ht,RS,startS,endS,RR,resList,1);		// 1 -> hashedRr
			deleteHashTable(&ht);
		}
	}
}
resultList* RadixHashJoin(oneColumnRelation *relR,oneColumnRelation *relS){


	hist *histSumArrayR,*histSumArrayS;
	oneColumnRelation *RS,*RR;
	resultList *resList;

	histSumArrayS=createSumHistArray(createHistArray(&relS));//dhmiourgia hist sum arrays
	histSumArrayR=createSumHistArray(createHistArray(&relR));

	RR=createReOrderedArray(relR,histSumArrayR);//dhmiourgia reordered array
	RS=createReOrderedArray(relS,histSumArrayS);

	resList=initializeResultList();


	//createHT_CompareBuckets(resList,histSumArrayR,histSumArrayS,RR,RS,size_A,size_B);
	indexHT* ht;
	int32_t startR=0,endR,startS=0,endS,counter = 0,cnt=0;
	while(1)
	{
		if(counter+1 > histSumArrayR->histSize)break;		//counter+1 giati einai h thesh gia to end ,dld an h thesh pou tha einai to end einai ektos oriwn break//
		startR = histSumArrayR->histArray[counter].count;
		startS = histSumArrayS->histArray[counter].count;
		if(counter+1==histSumArrayR->histSize){
			endR=relR->num_of_tuples-1;
			endS=relS->num_of_tuples-1;
		}else{
			endR = histSumArrayR->histArray[counter+1].count-1;
			endS = histSumArrayS->histArray[counter+1].count-1;
		}

		counter++;
		if(endR+1 == startR || endS+1 == startS) continue;  //elegxos an kapoio einai keno , opote den bgazei apotelesma
		cnt++;
		if((endR - startR) >= (endS - startS))//kanoume hash to pio mikro bucket
		{
			ht=createHashTable(RS,startS,endS);
			compareRelations(ht,RR,startR,endR,RS,resList,0);		// 0 -> hashedRs
			deleteHashTable(&ht);
		}
		else
		{
			ht=createHashTable(RR,startR,endR);
			compareRelations(ht,RS,startS,endS,RR,resList,1);		// 1 -> hashedRr
			deleteHashTable(&ht);
		}
	}
	///

	free(histSumArrayR->histArray);
	free(histSumArrayR);
	free(histSumArrayS->histArray);
	free(histSumArrayS);

	free(RS->tuples);
	free(RS);
	free(RR->tuples);
	free(RR);

	free(relS->tuples);
	free(relS);
	free(relR->tuples);
	free(relR);

	return resList;

}

void writeFile(uint32_t size_A,uint32_t size_B){
	FILE *fp;
	fp=fopen("input-files/input.txt","w");
	fprintf(fp,"%d %d\n",size_A,size_B);
	for(int32_t i=0;i<size_A;i++)
	{
		fprintf(fp,"%d,",i);
	}
	fprintf(fp,"\n");
	for(int32_t i=0;i<size_B;i++)
	{
		fprintf(fp,"%d,",i+95);
	}
	fclose(fp);
}

void readFile(int32_t A[],uint32_t *size_A,int32_t B[],uint32_t *size_B){
	FILE *fp;
	fp=fopen("input-files/input.txt","r");
	int32_t el1,el2;
	fscanf(fp,"%d %d",size_A,size_B);

	for(int32_t i=0;i<*size_A;i++){
		fscanf(fp,"%d,",&el1);
		A[i]=el1;
	}
	for(int32_t i=0;i<*size_B;i++){
		fscanf(fp,"%d,",&el2);
		B[i]=el2;
	}
	fclose(fp);
}

void deleteResultList(resultList * reslist){
	resultNode *temp,*temp2;
	temp=reslist->start;

	while(temp!=NULL){
		free(temp->row_Array);
		temp2=temp;
		temp=temp->next;
		free(temp2);
	}

	free(reslist);
}
void readWorkFile(char *filename,multiColumnRelation *relationArray)
{
	FILE *query_f;
	query_f=fopen(filename,"r");
    char c='a';
	char *queryString;
	int counter=0,phrase_size=100,middleResultsSize=100,i,columnIndx,relationIndex,flag=0,middleResultsCounter=0,j,leftRelationIndx,rightRelationIndx,leftColumnIndx,rightColumnIndx;
	queryString = malloc(phrase_size*sizeof(char));
	middleResults *middleResArray=malloc(middleResultsSize*sizeof(middleResults));
	queryDataIndex *data;
    while(c!=EOF)
    {
    	counter=0,phrase_size=100;
		c=fgetc(query_f);
		while(c!=EOF && c!='\n')
		{
			queryString[counter]=c;
			if(counter==phrase_size)
			{
				phrase_size*=2;
				queryString=realloc(queryString,phrase_size*sizeof(char));
			}
			counter++;
			c=fgetc(query_f);
		}
		queryString[counter]='\0';
		if(strcmp(queryString,"F") && c!=EOF)
		{
			oneColumnRelation *column;
			printf("\n\n%s",queryString);
			data=analyzeQuery(queryString);
		//	getchar();

			if(data->numPredFilter>0)				//kanoume prwta tis prakseis me arithmous
			{
			/*	printf("Filters=%d\n",data->numPredFilter);
				for(i=0;i<data->numPredFilter;i++)		//apo 0 mexri ton arithmo twn filtrwn pou uparxoun//
				{

					relationIndex = data->predFilterArray[i].relColumn->rel;
					columnIndx= data->predFilterArray[i].relColumn->col;
					relationIndex = data->QueryRelArray[relationIndex];
					flag=0;
					if(middleResultsCounter>0)			//exoume endiamesa apotelesmata
					{
						for(j=0;j<middleResultsCounter;j++)			//trexoume ton pinaka me ta relations kai psaxnoume an uparxei antistoixia//
						{
							if(middleResArray[j].relation==relationIndex)
							{
								column=setColumnFromMiddleArray(middleResArray,relationIndex,columnIndx,j,relationArray);
								//printf("Before 1\n");
								//printMiddleArray(middleResArray,middleResultsCounter);
								free(middleResArray[j].rowIds);
								middleResArray[j]=executeFilter(column,data->predFilterArray[i].value,data->predFilterArray[i].typeOperation,relationIndex);
								if(middleResArray[middleResultsCounter].relation==-1)
								{
									free(middleResArray[middleResultsCounter].rowIds);
									middleResultsCounter--;
								}
								//printf("After 1\n");
								//printMiddleArray(middleResArray,middleResultsCounter);
								printf("Filter[%d]-->%d/%d\n",i,middleResArray[middleResultsCounter-1].rowIdsNum,relationArray[relationIndex].rowCount);
								flag=1;
								break;
							}
						}
						if(flag==1)continue;
					}

					//edw tha erthei an den uparxei kapoio apotelesma apo to middleResults h an uparxoun apotelesmata sto middleResult pou den mas ikanopoioun.ara ftiaxnoume neo RowID sto middleResult
					column = setColumnFromFirstArray(relationArray,relationIndex,columnIndx);
					//printf("Before\n");
					//printMiddleArray(middleResArray,middleResultsCounter);
					middleResArray[middleResultsCounter++]=executeFilter(column,data->predFilterArray[i].value,data->predFilterArray[i].typeOperation,relationIndex);
					if(middleResArray[middleResultsCounter-1].relation==-1)
					{
						free(middleResArray[middleResultsCounter-1].rowIds);
						middleResultsCounter--;
					}
					//printf("After\n");
					//printMiddleArray(middleResArray,middleResultsCounter);
					printf("Filter[%d]-->%d/%d\n",i,middleResArray[middleResultsCounter-1].rowIdsNum,relationArray[relationIndex].rowCount);
				}*/
			}
			if(data->numPredJoinTwoRel>0)			//join 2 columns//
			{

				for(i=0;i<data->numPredJoinTwoRel;i++)
				{
					oneColumnRelation *leftColumn;
					oneColumnRelation *rightColumn;

					leftRelationIndx = data->twoRelationPredArray[i].left->rel;					//pernoume ta stoixeia apo thn domh pou krataei ta predications data //
					leftColumnIndx =data->twoRelationPredArray[i].left->col;
					rightRelationIndx = data->twoRelationPredArray[i].right->rel;
					rightColumnIndx= data->twoRelationPredArray[i].right->col;

					leftRelationIndx = data->QueryRelArray[leftRelationIndx];
					rightRelationIndx = data->QueryRelArray[rightRelationIndx];
					//printf("%d.%d = %d.%d  ,,,%d\n",leftRelationIndx,leftColumnIndx,rightRelationIndx,rightColumnIndx,middleResultsCounter);



					int leftColumnPosInMiddleArray=-1,rightColumnPosInMiddleArray=-1;
					if(middleResultsCounter>0)			//exoume endiamesa apotelesmata
					{
						for(j=0;j<middleResultsCounter;j++)			//trexoume ton pinaka me ta relations kai psaxnoume an uparxei antistoixia//
						{
							if(middleResArray[j].relation==leftRelationIndx)
							{
								printf("leftRelationIndex=%d leftColumnIndx=%d j=%d\n",leftRelationIndx,leftColumnIndx,j);
								leftColumn=setColumnFromMiddleArray(middleResArray,leftRelationIndx,leftColumnIndx,j,relationArray);	//dhmiourgia aristerhs sthlhs apo to middle results
								leftColumnPosInMiddleArray=j;			//h thesh ths aristerhs sthlhs mesa ston middleArray
							}
							if(middleResArray[j].relation==rightRelationIndx)
							{
								rightColumn=setColumnFromMiddleArray(middleResArray,rightRelationIndx,rightColumnIndx,j,relationArray);	//dhmiourgia deksias sthlhs apo to middle results
								rightColumnPosInMiddleArray=j;			//h thesh ths deksias sthlhs mesa ston middleArray
							}
						}
					}
					printf("%d.%d = %d.%d  \n",leftRelationIndx,leftColumnIndx,rightRelationIndx,rightColumnIndx);
					if(rightColumnPosInMiddleArray==-1)		//an den uparxei mesa ston middle array pairnoume thn sthlh apo ton arxiko pinaka
						rightColumn = setColumnFromFirstArray(relationArray,rightRelationIndx,rightColumnIndx);		//dhmiourgia deksias sthlhs apo ton arxiko pinaka
					if(leftColumnPosInMiddleArray==-1)
						leftColumn = setColumnFromFirstArray(relationArray,leftRelationIndx,leftColumnIndx);		//dhmiourgia aristerhs sthlhs apo ton arxiko pinaka


					resultList *resultList1,*resultList2;
					if(leftRelationIndx==rightRelationIndx)			//JoinoneRelationArray
					{
						printf("Before Join\n");
						//printMiddleArray(middleResArray,middleResultsCounter);
						resultList1=sameRelationJoin(leftColumn,rightColumn);
						printf("%d middle %d left %d right\n",middleResultsCounter,leftColumnPosInMiddleArray,rightColumnPosInMiddleArray);
						if(leftColumnPosInMiddleArray==-1)			//an den uparxei to left column ston middle array
						{
							printf("left -1\n");
							middleResArray[middleResultsCounter].relation=leftRelationIndx;
							setResultsToMiddleArray(resultList1,middleResArray,middleResultsCounter,"left");
							middleResultsCounter++;
						}
						/*if(rightColumnPosInMiddleArray==-1)			//an den uparxei to right column ston middle array
						{
							printf("right -1\n");
							middleResArray[middleResultsCounter].relation=rightRelationIndx;
							setResultsToMiddleArray(resultList1,middleResArray,middleResultsCounter,"right");
							middleResultsCounter++;
						}*/
						if(leftColumnPosInMiddleArray!=-1)			//an uparxei to left column ston middle array
						{
							printf("left +1\n");
							free(middleResArray[leftColumnPosInMiddleArray].rowIds);
							middleResArray[leftColumnPosInMiddleArray].relation=leftRelationIndx;
							setResultsToMiddleArray(resultList1,middleResArray,leftColumnPosInMiddleArray,"left");
						}
						/*if(rightColumnPosInMiddleArray!=-1)			//an uparxei to right column ston middle array
						{
							printf("right +1\n");
							free(middleResArray[rightColumnPosInMiddleArray].rowIds);
							middleResArray[rightColumnPosInMiddleArray].relation=rightRelationIndx;
							setResultsToMiddleArray(resultList1,middleResArray,rightColumnPosInMiddleArray,"right");
						}*/
						printf("After Join\n");
						//printMiddleArray(middleResArray,middleResultsCounter);
						//printResults(resultList2);
					}
					else											//JointwoRelationArray
					{
						printf("Before Join\n");
						printMiddleArray(middleResArray,middleResultsCounter);
						resultList2=RadixHashJoin(leftColumn,rightColumn);
						printf("%d middle %d left %d right\n",middleResultsCounter,leftColumnPosInMiddleArray,rightColumnPosInMiddleArray);
						if(leftColumnPosInMiddleArray==-1)			//an den uparxei to left column ston middle array
						{
							printf("left -1\n");
							middleResArray[middleResultsCounter].relation=leftRelationIndx;
							setResultsToMiddleArray(resultList2,middleResArray,middleResultsCounter,"left");
							middleResultsCounter++;
						}
						if(rightColumnPosInMiddleArray==-1)			//an den uparxei to right column ston middle array
						{
							printf("right -1\n");
							middleResArray[middleResultsCounter].relation=rightRelationIndx;
							setResultsToMiddleArray(resultList2,middleResArray,middleResultsCounter,"right");
							middleResultsCounter++;
						}
						if(leftColumnPosInMiddleArray!=-1)			//an uparxei to left column ston middle array
						{
							printf("left +1\n");
							free(middleResArray[leftColumnPosInMiddleArray].rowIds);
							middleResArray[leftColumnPosInMiddleArray].relation=leftRelationIndx;
							setResultsToMiddleArray(resultList2,middleResArray,leftColumnPosInMiddleArray,"left");
						}
						if(rightColumnPosInMiddleArray!=-1)			//an uparxei to right column ston middle array
						{
							printf("right +1\n");
							free(middleResArray[rightColumnPosInMiddleArray].rowIds);
							middleResArray[rightColumnPosInMiddleArray].relation=rightRelationIndx;
							setResultsToMiddleArray(resultList2,middleResArray,rightColumnPosInMiddleArray,"right");
						}
						printf("After Join\n");
						printMiddleArray(middleResArray,middleResultsCounter);
						//printResults(resultList2);
					}
					leftColumnPosInMiddleArray=-1;
					rightColumnPosInMiddleArray=-1;

				}
			}

			//free(middleResArray->rowIds);
			//free(middleResArray);
			/*printf("SUms-->%d\n",data->numViewQuery);
			if(data->numViewQuery>0)
			{
				for(i=0;i<data->numViewQuery;i++)
				{
					columnIndx = data->viewQueryArray[i].col;
					relationIndex =data->viewQueryArray[i].rel;
					relationIndex = data->QueryRelArray[relationIndex];
					for(j=0;j<middleResultsCounter;j++)
					{
						if(middleResArray[j].relation==relationIndex)
						{
							column=setColumnFromMiddleArray(middleResArray,relationIndex,columnIndx,j,relationArray);
							printf("Sum=%ld\n",SumOneColumnRelation(column));
						}
					}

				}
			}*/
			middleResultsCounter=0;
		}
		memset(queryString,0,strlen(queryString));
    }
}
void printMiddleArray(middleResults *array,int size)
{
	for(int i=0;i<size;i++)
	{
		printf("Relation %d  Rows %d\n",array[i].relation,array[i].rowIdsNum);
		for(int j=0;j<array[i].rowIdsNum;j++)
		{
			//printf("Row[%d]=%d\n",j,array[i].rowIds[j]);
		}
		//printf("\n");
	}
}
void setResultsToMiddleArray(resultList *list,middleResults *middleResultsArray,int index,char* direction)
{
	resultNode *temp;
	temp =list->start;
	if(temp==NULL)return;
	middleResultsArray[index].rowIdsNum=0;
	middleResultsArray[index].rowIds=malloc(temp->rowSize*sizeof(int));
	while(temp!=NULL){
		for(int i=0;i<temp->rowSize;i++){
			if(!strcmp(direction,"left")){
				middleResultsArray[index].rowIds[i]=temp->row_Array[i].idR;
				middleResultsArray[index].rowIdsNum++;
			}
			else{
				middleResultsArray[index].rowIds[i]=temp->row_Array[i].idS;
				middleResultsArray[index].rowIdsNum++;
			}
		}
		temp=temp->next;
	}
}
oneColumnRelation* setColumnFromFirstArray(multiColumnRelation* relationArray,int relationIndx,int columnIndx){
	oneColumnRelation *temp;
	temp=malloc(sizeof(oneColumnRelation));
	temp->tuples=malloc(relationArray[relationIndx].rowCount*sizeof(tuple));
	temp->num_of_tuples=relationArray[relationIndx].rowCount;
	for(int j=0;j<relationArray[relationIndx].rowCount;j++)
	{
		temp->tuples[j].value=relationArray[relationIndx].table[columnIndx][j];		//h mporw na stelnw kateutheian to table[column]//vazw se mia sthlh thn sthlh apo to arxiko table me ta data//
		temp->tuples[j].id=j;
	}
	return temp;
}
oneColumnRelation* setColumnFromMiddleArray(middleResults* middleResArray,int relationIndx,int columnIndx,int arrayIndx,multiColumnRelation* relationArray){
	oneColumnRelation *temp;
	temp=malloc(sizeof(oneColumnRelation));
	temp->tuples=malloc(middleResArray[arrayIndx].rowIdsNum*sizeof(tuple));
	temp->num_of_tuples=middleResArray[arrayIndx].rowIdsNum;
	for(int k=0;k<middleResArray[arrayIndx].rowIdsNum;k++)
	{
		int index=middleResArray[arrayIndx].rowIds[k];
		//printf("%d %d %d %d\n",index,columnIndx,relationArray[relationIndx].rowCount,relationArray[relationIndx].colCount);
		temp->tuples[k].value=relationArray[relationIndx].table[columnIndx][index];
		temp->tuples[k].id=index;
	}
	return temp;
}
queryDataIndex* analyzeQuery(char *query)
{
	char *token,*token2,*token3,*temp,*tempToken;
	char *tempView,*tok,*bufRel,*bufCol;
	int i,j,k,index,numRel=0,numView=0;
	int numPredFilter=0,numPredJoinTwoRel=0;

	token=strtok(query,"|");
	token2=strtok(NULL,"|");
	token3=strtok(NULL,"|");

	tempToken=malloc(128);
	tempView=malloc(128);

	queryDataIndex *queryData;
	queryData=malloc(sizeof(queryDataIndex));

	for(i=0;i<3;i++){
		memset(tempToken,0,128);
		if(i==0){//1st part of query
			token=strtok(query,"|");
			strcat(tempToken,token);
			temp=strtok(token," ");
			while(temp!=NULL){
				numRel++;
				temp=strtok(NULL," ");
			}
			queryData->numRelQuery=numRel;
			queryData->QueryRelArray=malloc(numRel*sizeof(int));
			temp=strtok(tempToken," ");
			for(j=0;j<numRel;j++){
				queryData->QueryRelArray[j]=atoi(temp);
				temp=strtok(NULL," ");
			}
		}
		else if(i==1){//2nd part of query
			strcat(tempToken,token2);
			temp=strtok(token2,"&");
			while(temp!=NULL){
				memset(tempView,0,128);
				strcat(tempView,temp);
				temp=strtok(NULL,"&");
				if((tok=strchr(tempView,'>'))!=NULL ||(tok=strchr(tempView,'<'))!=NULL || ( (tok=strchr(tempView,'='))!=NULL && (tok=strstr(tok,"."))==NULL) ){
				//	printf("> or <");
					numPredFilter++;
				}
				else if((tok=strchr(tempView,'='))!=NULL){//peritto else if mallon
						numPredJoinTwoRel++;
				}
				else{
					printf("wrong\n");
				}
			}
			//printf("pred=%d oneR=%d twoR=%d\n",numPredFilter,numPredJoinOneRel,numPredJoinTwoRel);

			queryData->numPredFilter=numPredFilter;
			queryData->predFilterArray=malloc(queryData->numPredFilter*sizeof(filterPredNode));
			for(k=0;k<queryData->numPredFilter;k++)
				queryData->predFilterArray[k].relColumn=malloc(sizeof(RelColNode));

			queryData->numPredJoinTwoRel=numPredJoinTwoRel;
			queryData->twoRelationPredArray=malloc(queryData->numPredJoinTwoRel*sizeof(joinPredNode));
			for(k=0;k<queryData->numPredJoinTwoRel;k++){
				queryData->twoRelationPredArray[k].left=malloc(sizeof(RelColNode));
				queryData->twoRelationPredArray[k].right=malloc(sizeof(RelColNode));
			}


			int countF=0,countTwoR=0;
			char oper,*bufVal,*tok2;
			temp=strtok(tempToken,"&");
			while(temp!=NULL){
				memset(tempView,0,128);
				strcat(tempView,temp);
				temp=strtok(NULL,"&");
				if((tok=strchr(tempView,'>'))!=NULL ||(tok=strchr(tempView,'<'))!=NULL || ( (tok=strchr(tempView,'='))!=NULL && (tok2=strstr(tok,"."))==NULL) ){
					//printf("tok %s\n",tok);
					index = (int)(tok - tempView);
					oper=tempView[index];
					bufVal=malloc(128);
					memset(bufVal,0,128);
					for(k=index+1;k<strlen(tempView);k++){
						bufVal[k-index-1]=tempView[k];
					}
					bufVal[strlen(tempView)]='\0';

					tok=strchr(tempView,'.');
					index = (int)(tok - tempView);

					bufRel=malloc(index);
					bufCol=malloc(index);

					for(k=0;k<index;k++){
						bufRel[k]=tempView[k];
						bufCol[k]=tempView[k+index+1];
					}
					bufRel[index]='\0';
					bufCol[index]='\0';
					//printf("R:%s C:%s\n",bufRel,bufCol);

					queryData->predFilterArray[countF].relColumn->rel=atoi(bufRel);
					queryData->predFilterArray[countF].relColumn->col=atoi(bufCol);
					queryData->predFilterArray[countF].typeOperation=oper;
					queryData->predFilterArray[countF].value=atoi(bufVal);

					countF++;
					free(bufVal);
					free(bufCol);
					free(bufRel);
				}
				else{//gia '='
					tok=strchr(tempView,'=');
					index = (int)(tok - tempView);
					int index_e=index;
					//printf("index=%d\n",index);

					bufVal=malloc(index);
					for(k=0;k<index;k++){
						bufVal[k]=tempView[k];
					}
					bufVal[index]='\0';
					tok=strchr(bufVal,'.');
					index = (int)(tok - bufVal);

					bufRel=malloc(index);
					bufCol=malloc(index);
					for(k=0;k<index;k++){
						bufRel[k]=bufVal[k];
						bufCol[k]=bufVal[k+index+1];
					}
					bufRel[index]='\0';
					bufCol[index]='\0';
				//	printf("R:%s C:%s\n",bufRel,bufCol);
					queryData->twoRelationPredArray[countTwoR].left->rel=atoi(bufRel);
					queryData->twoRelationPredArray[countTwoR].left->col=atoi(bufCol);

					free(bufRel);
					free(bufCol);
					free(bufVal);
					////////////////////////////////////////////////////////////////
					bufVal=malloc(index_e);
					for(k=index_e+1;k<strlen(tempView);k++){
						bufVal[k-index_e-1]=tempView[k];
					}
					bufVal[index_e]='\0';
					tok=strchr(bufVal,'.');
					index = (int)(tok - bufVal);
					bufRel=malloc(index);
					bufCol=malloc(index);
					for(k=0;k<index;k++){
						bufRel[k]=bufVal[k];
						bufCol[k]=bufVal[k+index+1];
					}
					bufRel[index]='\0';
					bufCol[index]='\0';
					//printf("R:%s C:%s\n",bufRel,bufCol);
					queryData->twoRelationPredArray[countTwoR].right->rel=atoi(bufRel);
					queryData->twoRelationPredArray[countTwoR].right->col=atoi(bufCol);
					//////////////////////////////////////////////////////////////
					countTwoR++;
					free(bufRel);
					free(bufCol);
					free(bufVal);

				}
			}
		}
		else{//3rd part of query
			//printf("View: ");
			strcat(tempToken,token3);
			temp=strtok(token3," ");
			while(temp!=NULL){
				numView++;
				temp=strtok(NULL," ");
			}
			queryData->numViewQuery=numView;
			queryData->viewQueryArray=malloc(numView*sizeof(RelColNode));
			temp=strtok(tempToken," ");
			for(j=0;j<numView;j++){
				memset(tempView,0,128);
				strcat(tempView,temp);
				temp=strtok(NULL," ");

				tok = strchr(tempView, '.');
				index = (int)(tok - tempView);

				bufRel=malloc(index);
				bufCol=malloc(index);

				for(k=0;k<index;k++){
					bufRel[k]=tempView[k];
					bufCol[k]=tempView[k+index+1];
				}
				bufRel[index]='\0';
				bufCol[index]='\0';
				queryData->viewQueryArray[j].rel=atoi(bufRel);
				queryData->viewQueryArray[j].col=atoi(bufCol);
				free(bufCol);
				free(bufRel);
			}
		}
	}

	free(tempToken);
	free(tempView);
	printf("\n");
	return queryData;
}
resultList* sameRelationJoin(oneColumnRelation *relR,oneColumnRelation *relS){//exoun to idio size
	int i;
	resultList *resList;
	resList=initializeResultList();

	for(i=0;i<relR->num_of_tuples;i++){
		if(relR->tuples[i].value==relS->tuples[i].value){//prepei ta id na einai ta idia
			//printf("relR->tuples[i].id = %d\n",relR->tuples[i].id);
			insertResult(resList,relR->tuples[i].id,relR->tuples[i].id,2);			//2 sto teleutaio orisma shmainei oti exoume join apo idio relation
		}
	}
	return resList;
}
middleResults executeFilter(oneColumnRelation* column,int value,char typeOperation,int relation)
{
	int i;
	middleResults results;

	results.rowIds=malloc(column->num_of_tuples*sizeof(int));				//isws einai megalos o xwros apothikeushs//
	results.rowIdsNum=0;
	results.relation=-1;
	for(i=0;i<column->num_of_tuples;i++)
	{
		if(typeOperation=='=')
		{	if(column->tuples[i].value==value){
				results.rowIds[results.rowIdsNum]=column->tuples[i].id;
				results.relation = relation;
				results.rowIdsNum++;
			}
		}
		else if(typeOperation=='>')
		{
			if(column->tuples[i].value>value){
				results.rowIds[results.rowIdsNum]=column->tuples[i].id;
				results.relation = relation;
				results.rowIdsNum++;
			}
		}
		else if(typeOperation=='<')
		{
			if(column->tuples[i].value<value){
				results.rowIds[results.rowIdsNum]=column->tuples[i].id;
				results.relation = relation;
				results.rowIdsNum++;
			}
		}
	}
	return results;
}
/*resultListForJoin *initializeResultListForJoin(void){
	resultListForJoin *list=malloc(sizeof(resultListForJoin));
	list->start=NULL;
	list->end=NULL;
	list->numberOfNodes=0;
	list->numberOfResults=0;
	return list;
}
void insertResultForJoin(resultListForJoin *list,uint32_t id){
	int32_t numberoftuples=(1024*1024)/sizeof(int32_t);
	//printf("numberOfResults=%d  %ld\n",numberoftuples,sizeof(int32_t));
	if(list->end==NULL){//kenh lista
		list->start=malloc(sizeof(resultNodeForJoin));
		list->end=list->start;
		list->numberOfNodes=1;
		list->numberOfResults=1;

		list->start->row_Array=malloc(numberoftuples*sizeof(int32_t));
		list->start->next=NULL;
		list->start->rowSize=1;
		list->start->row_Array[0]=id;
	}
	else{
		list->numberOfResults++;
		if( numberoftuples > list->end->rowSize ){//exei xwro

			list->end->row_Array[list->end->rowSize]=id;
			list->end->rowSize++;
		}
		else{//ftiaxnw neo kombo
			list->end->next=malloc(sizeof(resultNodeForJoin));
			list->end=list->end->next;
			list->numberOfNodes++;

			list->end->row_Array=malloc(numberoftuples*sizeof(int32_t));
			list->end->next=NULL;
			list->end->rowSize=1;

			list->end->row_Array[0]=id;
		}
	}
}
void printResultsForJoin(resultListForJoin *list){
	resultNodeForJoin *temp;
	temp =list->start;
	printf("\nRowid\n");
	while(temp!=NULL){
		for(int i=0;i<temp->rowSize;i++){
			printf("%d\n",temp->row_Array[i]);
		}
		temp=temp->next;
	}
	printf("\n%d Results\n\n",list->numberOfResults);
}
*/
int64_t SumOneColumnRelation(oneColumnRelation *R){
	int sum=0,i;

	for(i=0;i<R->num_of_tuples;i++){
		sum+=R->tuples[i].value;
	}

	return sum;
}









