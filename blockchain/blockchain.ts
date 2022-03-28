class Blockchain{
    chain : Array<Object>;

    constructor () {
        this.createNewBlock(100, "0", "0", "0");
    }

    public createNewBlock = (nonce: number, previousBlockHash : string, hash : string, data : string) => {
        const newBlock = {
            index : this.chain.length + 1,
            timestamp : Date.now(),
            nonce : nonce, // Proof of work
            hash : hash,
            data : data, // fingerprint
            previousBlockHash : previousBlockHash // hash of value of previous block in blockchain

        }

        this.chain.push(newBlock);
    }

    public getLastBlock = () => {
        return this.chain[this.chain.length - 1];
    }

    public generateHash = (previousBlockHash : string, blockData : string, nonce : number) => {
        const dataAsString = previousBlockHash + blockData + nonce;
        const hash = sha256(dataAsString);
        return hash;
    }

    public proofOfWork = (previousBlockHash : string, currentBlockData) => {
        let nonce = 0;
        let hash = this.generateHash(previousBlockHash, currentBlockData, nonce);
        while(hash.substring(0,4) === "0000" ){
            nonce++;
            hash = this.generateHash(previousBlockHash, currentBlockData, nonce);
        }
        return nonce;
    }
}

