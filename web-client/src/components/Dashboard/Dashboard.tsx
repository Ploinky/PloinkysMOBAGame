import React, { FormEvent } from 'react';
import { NativeCaller } from '../../utils/NativeCaller';
import { Button } from '../Button/Button';
import { FlexContainer } from '../FlexContainer/FlexContainer';

function Dashboard() {
    const onSubmit = (e: FormEvent<HTMLFormElement>) => {
        e.preventDefault();

        const ip = (e.target as any).ip.value;

        NativeCaller.connect(ip);
    }
    return (
        <FlexContainer direction="column" justify="space-around">
            <form onSubmit={onSubmit}>
                <FlexContainer direction="column">
                    <input name="ip" placeholder="Server IP"/>
                    <Button type="submit">Connect</Button>
                </FlexContainer>
            </form>
        </FlexContainer>
    )
}

export { Dashboard };